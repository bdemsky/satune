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
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	ourfree(This);
}

Constraint * getElementValueConstraint(SATEncoder* encoder,Element* This, uint64_t value) {
	generateElementEncodingVariables(encoder, getElementEncoding(This));
	switch(getElementEncoding(This)->type){
		case ONEHOT:
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

void encodeAllSATEncoder(CSolver *csolver, SATEncoder * This) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		Constraint* c= encodeConstraintSATEncoder(This, constraint);
		printConstraint(c);
		model_print("\n");
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
		pair = &flipped;
	}
	Constraint * constraint;
	if (!containsBoolConst(table, pair)) {
		constraint = getNewVarSATEncoder(This);
		OrderPair * paircopy = allocOrderPair(pair->first, pair->second);
		putBoolConst(table, paircopy, constraint);
	} else
		constraint = getBoolConst(table, pair);
	if (negate)
		return negateConstraint(constraint);
	else
		return constraint;
	
}

Constraint * encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * boolOrder){
	ASSERT(boolOrder->order->type == TOTAL);
	HashTableBoolConst* boolToConsts = boolOrder->order->boolsToConstraints;
	OrderPair pair={boolOrder->first, boolOrder->second};
	Constraint* constraint = getPairConstraint(This, boolToConsts, & pair);
	ASSERT(constraint != NULL);
	return constraint;
}

void createAllTotalOrderConstraintsSATEncoder(SATEncoder* This, Order* order){
	ASSERT(order->type == TOTAL);
	VectorInt* mems = order->set->members;
	HashTableBoolConst* table = order->boolsToConstraints;
	uint size = getSizeVectorInt(mems);
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
				generateTransOrderConstraintSATEncoder(This, constIJ, constJK, constIK); 
			}
		}
	}
}

Constraint* getOrderConstraint(HashTableBoolConst *table, OrderPair *pair){
	ASSERT(pair->first!= pair->second);
	Constraint* constraint= getBoolConst(table, pair);
	ASSERT(constraint!= NULL);
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
			if( GETELEMENTTYPE(el) == ELEMFUNCRETURN)
				encodeFunctionElementSATEncoder(This, (ElementFunction*) el);
			carray[j] = getElementValueConstraint(This,el, entry->inputs[j]);
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
	if( GETELEMENTTYPE(elem1) == ELEMFUNCRETURN)
		encodeFunctionElementSATEncoder(This, (ElementFunction*) elem1);
	Element* elem2 = getArrayElement( &constraint->inputs, 1);
	if( GETELEMENTTYPE(elem2) == ELEMFUNCRETURN)
		encodeFunctionElementSATEncoder(This, (ElementFunction*) elem2);
	for(uint i=0; i<size; i++){
		Constraint* arg1 = getElementValueConstraint(This, elem1, commonElements[i]);
		ASSERT(arg1!=NULL);
		Constraint* arg2 = getElementValueConstraint(This, elem2, commonElements[i]);
		ASSERT(arg2 != NULL);
		carray[i] =  allocConstraint(AND, arg1, arg2);
	}
	//FIXME: the case when there is no intersection ....
	return allocArrayConstraint(OR, size, carray);
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
	generateElementEncodingVariables(encoder, elem1);
	ElementEncoding* elem2 = getElementEncoding( getArrayElement(&This->inputs,1) );
	generateElementEncodingVariables(encoder, elem2);
	Constraint* carray[elem1->encArraySize*elem2->encArraySize];
	uint size=0;
	Constraint* overFlowConstraint = ((BooleanVar*) This->overflowstatus)->var;
	for(uint i=0; i<elem1->encArraySize; i++){
		if(isinUseElement(elem1, i)){
			for(uint j=0; j<elem2->encArraySize; j++){
				if(isinUseElement(elem2, j)){
					bool isInRange = false, hasOverFlow=false;
					uint64_t result= applyFunctionOperator((FunctionOperator*)This->function,elem1->encodingArray[i],
						elem2->encodingArray[j], &isInRange, &hasOverFlow);
					if(!isInRange)
						break; // Ignoring the cases that result of operation doesn't exist in the code.
					//FIXME: instead of getElementValueConstraint, it might be useful to have another function
					// that doesn't iterate over encodingArray and treats more efficient ...
					Constraint* and1 = getElementValueConstraint(encoder, elem1->element, elem1->encodingArray[i]);
					ASSERT(and1 != NULL);
					Constraint* and2 = getElementValueConstraint(encoder, elem2->element, elem2->encodingArray[j]);
					ASSERT(and2 != NULL);
					Constraint* imply2 = getElementValueConstraint(encoder, (Element*) This, result);
					ASSERT(imply2 != NULL);
					Constraint* constraint = allocConstraint(IMPLIES, 
						allocConstraint(AND, and1, and2) , imply2);
					switch( ((FunctionOperator*)This->function)->overflowbehavior ){
						case IGNORE:
							if(!hasOverFlow){
								carray[size++] = constraint;
							}
							break;
						case WRAPAROUND:
							carray[size++] = constraint;
							break;
						case FLAGFORCESOVERFLOW:
							if(hasOverFlow){
								Constraint* const1 = allocConstraint(IMPLIES, overFlowConstraint, 
										allocConstraint(AND, getElementValueConstraint(encoder, elem1->element, elem1->encodingArray[i]),
										getElementValueConstraint(encoder, elem2->element, elem2->encodingArray[j])));
								carray[size++] = allocConstraint(AND, const1, constraint);
							}
							break;
						case OVERFLOWSETSFLAG:
							if(hasOverFlow){
								Constraint* const1 = allocConstraint(IMPLIES, allocConstraint(AND, getElementValueConstraint(encoder, elem1->element, elem1->encodingArray[i]),
										getElementValueConstraint(encoder, elem2->element, elem2->encodingArray[j])), overFlowConstraint);
								carray[size++] = allocConstraint(AND, const1, constraint);
							} else
								carray[size++] = constraint;
							break;
						case FLAGIFFOVERFLOW:
							if(!hasOverFlow){
								carray[size++] = constraint;
							}else{
								Constraint* const1 = allocConstraint(IMPLIES, overFlowConstraint, 
										allocConstraint(AND, getElementValueConstraint(encoder, elem1->element, elem1->encodingArray[i]),
										getElementValueConstraint(encoder, elem2->element, elem2->encodingArray[j])));
								Constraint* const2 = allocConstraint(IMPLIES, allocConstraint(AND, getElementValueConstraint(encoder, elem1->element, elem1->encodingArray[i]),
										getElementValueConstraint(encoder, elem2->element, elem2->encodingArray[j])), overFlowConstraint);
								Constraint* res [] = {const1, const2, constraint};
								carray[size++] = allocArrayConstraint(AND, 3, res);
							}
							break;
						case NOOVERFLOW:
							if(hasOverFlow){
								ASSERT(0);
							}
							carray[size++] = constraint;
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
