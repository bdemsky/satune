#include "satencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "constraint.h"
#include "common.h"
#include "element.h"
#include "function.h"
#include "tableentry.h"
#include "table.h"
#include "order.h"
#include "predicate.h"
#include "orderpair.h"
#include "set.h"

SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
	This->varcount=1;
	This->satSolver = allocIncrementalSolver();
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	deleteIncrementalSolver(This->satSolver);
	ourfree(This);
}

Constraint * getElementValueConstraint(SATEncoder* encoder,Element* This, uint64_t value) {
	generateElementEncodingVariables(encoder, getElementEncoding(This));
	switch(getElementEncoding(This)->type){
		case ONEHOT:
			//FIXME
			ASSERT(0);
			break;
		case UNARY:
			ASSERT(0);
			break;
		case BINARYINDEX:
			return getElementValueBinaryIndexConstraint(This, value);
			break;
		case ONEHOTBINARY:
			ASSERT(0);
			break;
		case BINARYVAL:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
			break;
	}
	return NULL;
}
Constraint * getElementValueBinaryIndexConstraint(Element* This, uint64_t value) {
	ASTNodeType type = GETELEMENTTYPE(This);
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding* elemEnc = getElementEncoding(This);
	for(uint i=0; i<elemEnc->encArraySize; i++){
		if( isinUseElement(elemEnc, i) && elemEnc->encodingArray[i]==value){
			return generateBinaryConstraint(elemEnc->numVars,
				elemEnc->variables, i);
		}
	}
	return NULL;
}

void addConstraintToSATSolver(Constraint *c, IncrementalSolver* satSolver) {
	VectorConstraint* simplified = simplifyConstraint(c);
	uint size = getSizeVectorConstraint(simplified);
	for(uint i=0; i<size; i++) {
		Constraint *simp=getVectorConstraint(simplified, i);
		if (simp->type==TRUE)
			continue;
		ASSERT(simp->type!=FALSE);
		dumpConstraint(simp, satSolver);
		freerecConstraint(simp);
	}
	deleteVectorConstraint(simplified);
}

void encodeAllSATEncoder(CSolver *csolver, SATEncoder * This) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		Constraint* c= encodeConstraintSATEncoder(This, constraint);
		printConstraint(c);
		model_print("\n\n");
		addConstraintToSATSolver(c, This->satSolver);
		//FIXME: When do we want to delete constraints? Should we keep an array of them
		// and delete them later, or it would be better to just delete them right away?
	}
}

Constraint * encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint) {
	switch(GETBOOLEANTYPE(constraint)) {
	case ORDERCONST:
		return encodeOrderSATEncoder(This, (BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder(This, (BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder(This, (BooleanLogic *) constraint);
	case PREDICATEOP:
		return encodePredicateSATEncoder(This, (BooleanPredicate *) constraint);
	default:
		model_print("Unhandled case in encodeConstraintSATEncoder %u", GETBOOLEANTYPE(constraint));
		exit(-1);
	}
}

void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Constraint **carray) {
	for(uint i=0;i<num;i++)
		carray[i]=getNewVarSATEncoder(encoder);
}

Constraint * getNewVarSATEncoder(SATEncoder *This) {
	Constraint * var=allocVarConstraint(VAR, This->varcount);
	Constraint * varneg=allocVarConstraint(NOTVAR, This->varcount++);
	setNegConstraint(var, varneg);
	setNegConstraint(varneg, var);
	return var;
}

Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint) {
	if (constraint->var == NULL) {
		constraint->var=getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint) {
	Constraint * array[getSizeArrayBoolean(&constraint->inputs)];
	for(uint i=0;i<getSizeArrayBoolean(&constraint->inputs);i++)
		array[i]=encodeConstraintSATEncoder(This, getArrayBoolean(&constraint->inputs, i));

	switch(constraint->op) {
	case L_AND:
		return allocArrayConstraint(AND, getSizeArrayBoolean(&constraint->inputs), array);
	case L_OR:
		return allocArrayConstraint(OR, getSizeArrayBoolean(&constraint->inputs), array);
	case L_NOT:
		ASSERT( getSizeArrayBoolean(&constraint->inputs)==1);
		return negateConstraint(array[0]);
	case L_XOR: {
		ASSERT( getSizeArrayBoolean(&constraint->inputs)==2);
		Constraint * nleft=negateConstraint(cloneConstraint(array[0]));
		Constraint * nright=negateConstraint(cloneConstraint(array[1]));
		return allocConstraint(OR,
													 allocConstraint(AND, array[0], nright),
													 allocConstraint(AND, nleft, array[1]));
	}
	case L_IMPLIES:
		ASSERT( getSizeArrayBoolean( &constraint->inputs)==2);
		return allocConstraint(IMPLIES, array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}


Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint) {
	switch( constraint->order->type){
		case PARTIAL:
			return encodePartialOrderSATEncoder(This, constraint);
		case TOTAL:
			return encodeTotalOrderSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint * getPairConstraint(SATEncoder *This, HashTableBoolConst * table, OrderPair * pair) {
	bool negate = false;
	OrderPair flipped;
	if (pair->first > pair->second) {
		negate=true;
		flipped.first=pair->second;
		flipped.second=pair->first;
		pair = &flipped;	//FIXME: accessing a local variable from outside of the function?
	}
	Constraint * constraint;
	if (!containsBoolConst(table, pair)) {
		constraint = getNewVarSATEncoder(This);
		OrderPair * paircopy = allocOrderPair(pair->first, pair->second, constraint);
		putBoolConst(table, paircopy, paircopy);
	} else
		constraint = getBoolConst(table, pair)->constraint;
	if (negate)
		return negateConstraint(constraint);
	else
		return constraint;
	
}

Constraint * encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * boolOrder){
	ASSERT(boolOrder->order->type == TOTAL);
	if(boolOrder->order->boolsToConstraints == NULL){
		initializeOrderHashTable(boolOrder->order);
		return createAllTotalOrderConstraintsSATEncoder(This, boolOrder->order);
	}
	HashTableBoolConst* boolToConsts = boolOrder->order->boolsToConstraints;
	OrderPair pair={boolOrder->first, boolOrder->second, NULL};
	Constraint *constraint = getPairConstraint(This, boolToConsts, & pair);
	return constraint;
}

Constraint* createAllTotalOrderConstraintsSATEncoder(SATEncoder* This, Order* order){
	ASSERT(order->type == TOTAL);
	VectorInt* mems = order->set->members;
	HashTableBoolConst* table = order->boolsToConstraints;
	uint size = getSizeVectorInt(mems);
	Constraint* constraints [size*size];
	uint csize =0;
	for(uint i=0; i<size; i++){
		uint64_t valueI = getVectorInt(mems, i);
		for(uint j=i+1; j<size;j++){
			uint64_t valueJ = getVectorInt(mems, j);
			OrderPair pairIJ = {valueI, valueJ};
			Constraint* constIJ=getPairConstraint(This, table, & pairIJ);
			for(uint k=j+1; k<size; k++){
				uint64_t valueK = getVectorInt(mems, k);
				OrderPair pairJK = {valueJ, valueK};
				OrderPair pairIK = {valueI, valueK};
				Constraint* constIK = getPairConstraint(This, table, & pairIK);
				Constraint* constJK = getPairConstraint(This, table, & pairJK);
				constraints[csize++] = generateTransOrderConstraintSATEncoder(This, constIJ, constJK, constIK); 
				ASSERT(csize < size*size);
			}
		}
	}
	return allocArrayConstraint(AND, csize, constraints);
}

Constraint* getOrderConstraint(HashTableBoolConst *table, OrderPair *pair){
	ASSERT(pair->first!= pair->second);
	Constraint* constraint= getBoolConst(table, pair)->constraint;
	if(pair->first > pair->second)
		return constraint;
	else
		return negateConstraint(constraint);
}

Constraint * generateTransOrderConstraintSATEncoder(SATEncoder *This, Constraint *constIJ,Constraint *constJK,Constraint *constIK){
	//FIXME: first we should add the the constraint to the satsolver!
	ASSERT(constIJ!= NULL && constJK != NULL && constIK != NULL);
	Constraint *carray[] = {constIJ, constJK, negateConstraint(constIK)};
	Constraint * loop1= allocArrayConstraint(OR, 3, carray);
	Constraint * carray2[] = {negateConstraint(constIJ), negateConstraint(constJK), constIK};
	Constraint * loop2= allocArrayConstraint(OR, 3,carray2 );
	return allocConstraint(AND, loop1, loop2);
}

Constraint * encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint){
	// FIXME: we can have this implementation for partial order. Basically,
	// we compute the transitivity between two order constraints specified by the client! (also can be used
	// when client specify sparse constraints for the total order!)
	ASSERT(constraint->order->type == PARTIAL);
/*
	HashTableBoolConst* boolToConsts = boolOrder->order->boolsToConstraints;
	if( containsBoolConst(boolToConsts, boolOrder) ){
		return getBoolConst(boolToConsts, boolOrder);
	} else {
		Constraint* constraint = getNewVarSATEncoder(This); 
		putBoolConst(boolToConsts,boolOrder, constraint);
		VectorBoolean* orderConstrs = &boolOrder->order->constraints;
		uint size= getSizeVectorBoolean(orderConstrs);
		for(uint i=0; i<size; i++){
			ASSERT(GETBOOLEANTYPE( getVectorBoolean(orderConstrs, i)) == ORDERCONST );
			BooleanOrder* tmp = (BooleanOrder*)getVectorBoolean(orderConstrs, i);
			BooleanOrder* newBool;
			Constraint* first, *second;
			if(tmp->second==boolOrder->first){
				newBool = (BooleanOrder*)allocBooleanOrder(tmp->order,tmp->first,boolOrder->second);
				first = encodeTotalOrderSATEncoder(This, tmp);
				second = constraint;
				
			}else if (boolOrder->second == tmp->first){
				newBool = (BooleanOrder*)allocBooleanOrder(tmp->order,boolOrder->first,tmp->second);
				first = constraint;
				second = encodeTotalOrderSATEncoder(This, tmp);
			}else
				continue;
			Constraint* transConstr= encodeTotalOrderSATEncoder(This, newBool);
			generateTransOrderConstraintSATEncoder(This, first, second, transConstr );
		}
		return constraint;
	}
*/	
	return NULL;
}

Constraint * encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	switch(GETPREDICATETYPE(constraint->predicate) ){
		case TABLEPRED:
			return encodeTablePredicateSATEncoder(This, constraint);
		case OPERATORPRED:
			return encodeOperatorPredicateSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint * encodeTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	switch(constraint->encoding.type){
		case ENUMERATEIMPLICATIONS:
		case ENUMERATEIMPLICATIONSNEGATE:
			return encodeEnumTablePredicateSATEncoder(This, constraint);
		case CIRCUIT:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint * encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	VectorTableEntry* entries = &(((PredicateTable*)constraint->predicate)->table->entries);
	FunctionEncodingType encType = constraint->encoding.type;
	uint size = getSizeVectorTableEntry(entries);
	Constraint* constraints[size];
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(entries, i);
		if(encType==ENUMERATEIMPLICATIONS && entry->output!= true)
			continue;
		else if(encType==ENUMERATEIMPLICATIONSNEGATE && entry->output !=false)
			continue;
		ArrayElement* inputs = &constraint->inputs;
		uint inputNum =getSizeArrayElement(inputs);
		Constraint* carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el = getArrayElement(inputs, j);
			Constraint* tmpc = getElementValueConstraint(This,el, entry->inputs[j]);
			ASSERT(tmpc!= NULL);
			if( GETELEMENTTYPE(el) == ELEMFUNCRETURN){
				Constraint* func =encodeFunctionElementSATEncoder(This, (ElementFunction*) el);
				ASSERT(func!=NULL);
				carray[j] = allocConstraint(AND, func, tmpc);
			} else {
				carray[j] = tmpc;
			}
			ASSERT(carray[j]!= NULL);
		}
		constraints[i]=allocArrayConstraint(AND, inputNum, carray);
	}
	Constraint* result= allocArrayConstraint(OR, size, constraints);
	//FIXME: if it didn't match with any entry
	return encType==ENUMERATEIMPLICATIONS? result: negateConstraint(result);
}

Constraint * encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	switch(constraint->encoding.type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumOperatorPredicateSATEncoder(This, constraint);
		case CIRCUIT:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint * encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	ASSERT(GETPREDICATETYPE(constraint->predicate)==OPERATORPRED);
	PredicateOperator* predicate = (PredicateOperator*)constraint->predicate;
	ASSERT(predicate->op == EQUALS); //For now, we just only support equals
	//getting maximum size of in common elements between two sets!
	uint size=getSizeVectorInt( getArraySet( &predicate->domains, 0)->members);
	uint64_t commonElements [size];
	getEqualitySetIntersection(predicate, &size, commonElements);
	Constraint*  carray[size];
	Element* elem1 = getArrayElement( &constraint->inputs, 0);
	Constraint *elemc1 = NULL, *elemc2 = NULL;
	if( GETELEMENTTYPE(elem1) == ELEMFUNCRETURN)
		elemc1 = encodeFunctionElementSATEncoder(This, (ElementFunction*) elem1);
	Element* elem2 = getArrayElement( &constraint->inputs, 1);
	if( GETELEMENTTYPE(elem2) == ELEMFUNCRETURN)
		elemc2 = encodeFunctionElementSATEncoder(This, (ElementFunction*) elem2);
	for(uint i=0; i<size; i++){
		Constraint* arg1 = getElementValueConstraint(This, elem1, commonElements[i]);
		ASSERT(arg1!=NULL);
		Constraint* arg2 = getElementValueConstraint(This, elem2, commonElements[i]);
		ASSERT(arg2 != NULL);
		carray[i] =  allocConstraint(AND, arg1, arg2);
	}
	//FIXME: the case when there is no intersection ....
	Constraint* result = allocArrayConstraint(OR, size, carray);
	ASSERT(result!= NULL);
	if(elemc1!= NULL)
		result = allocConstraint(AND, result, elemc1);
	if(elemc2 != NULL)
		result = allocConstraint (AND, result, elemc2);
	return result;
}

Constraint* encodeFunctionElementSATEncoder(SATEncoder* encoder, ElementFunction *This){
	switch(GETFUNCTIONTYPE(This->function)){
		case TABLEFUNC:
			return encodeTableElementFunctionSATEncoder(encoder, This);
		case OPERATORFUNC:
			return encodeOperatorElementFunctionSATEncoder(encoder, This);
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint* encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	switch(getElementFunctionEncoding(This)->type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumTableElemFunctionSATEncoder(encoder, This);
			break;
		case CIRCUIT:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint* encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function) == OPERATORFUNC);
	ASSERT(getSizeArrayElement(&This->inputs)==2 );
	ElementEncoding* elem1 = getElementEncoding( getArrayElement(&This->inputs,0) );
	ElementEncoding* elem2 = getElementEncoding( getArrayElement(&This->inputs,1) );
	Constraint* carray[elem1->encArraySize*elem2->encArraySize];
	uint size=0;
	Constraint* overFlowConstraint = ((BooleanVar*) This->overflowstatus)->var;
	for(uint i=0; i<elem1->encArraySize; i++){
		if(isinUseElement(elem1, i)){
			for( uint j=0; j<elem2->encArraySize; j++){
				if(isinUseElement(elem2, j)){
					bool isInRange = false;
					uint64_t result= applyFunctionOperator((FunctionOperator*)This->function,elem1->encodingArray[i],
						elem2->encodingArray[j], &isInRange);
					//FIXME: instead of getElementValueConstraint, it might be useful to have another function
					// that doesn't iterate over encodingArray and treats more efficient ...
					Constraint* valConstrIn1 = getElementValueConstraint(encoder, elem1->element, elem1->encodingArray[i]);
					ASSERT(valConstrIn1 != NULL);
					Constraint* valConstrIn2 = getElementValueConstraint(encoder, elem2->element, elem2->encodingArray[j]);
					ASSERT(valConstrIn2 != NULL);
					Constraint* valConstrOut = getElementValueConstraint(encoder, (Element*) This, result);
					if(valConstrOut == NULL)
						continue; //FIXME:Should talk to brian about it!
					Constraint* OpConstraint = allocConstraint(IMPLIES, 
						allocConstraint(AND, valConstrIn1, valConstrIn2) , valConstrOut);
					switch( ((FunctionOperator*)This->function)->overflowbehavior ){
						case IGNORE:
							if(isInRange){
								carray[size++] = OpConstraint;
							}
							break;
						case WRAPAROUND:
							carray[size++] = OpConstraint;
							break;
						case FLAGFORCESOVERFLOW:
							if(isInRange){
								Constraint* const1 = allocConstraint(IMPLIES,
									allocConstraint(AND, valConstrIn1, valConstrIn2), 
									negateConstraint(overFlowConstraint));
								carray[size++] = allocConstraint(AND, const1, OpConstraint);
							}
							break;
						case OVERFLOWSETSFLAG:
							if(isInRange){
								carray[size++] = OpConstraint;
							} else{
								carray[size++] = allocConstraint(IMPLIES,
									allocConstraint(AND, valConstrIn1, valConstrIn2),
									overFlowConstraint);
							}
							break;
						case FLAGIFFOVERFLOW:
							if(isInRange){
								Constraint* const1 = allocConstraint(IMPLIES,
									allocConstraint(AND, valConstrIn1, valConstrIn2), 
									negateConstraint(overFlowConstraint));
								carray[size++] = allocConstraint(AND, const1, OpConstraint);
							}else{
								carray[size++] = allocConstraint(IMPLIES,
									allocConstraint(AND, valConstrIn1, valConstrIn2),
									overFlowConstraint);
							}
							break;
						case NOOVERFLOW:
							if(!isInRange){
								ASSERT(0);
							}
							carray[size++] = OpConstraint;
							break;
						default:
							ASSERT(0);
					}
					
				}
			}
		}
	}
	return allocArrayConstraint(AND, size, carray);
}

Constraint* encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function)==TABLEFUNC);
	ArrayElement* elements= &This->inputs;
	Table* table = ((FunctionTable*) (This->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	Constraint* constraints[size]; //FIXME: should add a space for the case that didn't match any entries
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		uint inputNum =getSizeArrayElement(elements);
		Constraint* carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el= getArrayElement(elements, j);
			carray[j] = getElementValueConstraint(encoder, el, entry->inputs[j]);
			ASSERT(carray[j]!= NULL);
		}
		Constraint* output = getElementValueConstraint(encoder, (Element*)This, entry->output);
		ASSERT(output!= NULL);
		Constraint* row= allocConstraint(IMPLIES, allocArrayConstraint(AND, inputNum, carray), output);
		constraints[i]=row;
	}
	Constraint* result = allocArrayConstraint(OR, size, constraints);
	return result;
}
