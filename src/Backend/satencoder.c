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
	This->cnf=createCNF();
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	deleteCNF(This->cnf);
	ourfree(This);
}

Edge getElementValueConstraint(SATEncoder* This, Element* elem, uint64_t value) {
	generateElementEncodingVariables(This, getElementEncoding(elem));
	switch(getElementEncoding(elem)->type){
		case ONEHOT:
			//FIXME
			ASSERT(0);
			break;
		case UNARY:
			ASSERT(0);
			break;
		case BINARYINDEX:
			return getElementValueBinaryIndexConstraint(This, elem, value);
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
	return E_BOGUS;
}

Edge getElementValueBinaryIndexConstraint(SATEncoder * This, Element* elem, uint64_t value) {
	ASTNodeType type = GETELEMENTTYPE(elem);
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding* elemEnc = getElementEncoding(elem);
	for(uint i=0; i<elemEnc->encArraySize; i++){
		if( isinUseElement(elemEnc, i) && elemEnc->encodingArray[i]==value){
			return generateBinaryConstraint(This->cnf, elemEnc->numVars, elemEnc->variables, i);
		}
	}
	return E_BOGUS;
}

void encodeAllSATEncoder(CSolver *csolver, SATEncoder * This) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		Edge c= encodeConstraintSATEncoder(This, constraint);
		printCNF(c);
		printf("\n\n");
		addConstraint(This->cnf, c);
	}
}

Edge encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint) {
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

void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Edge * carray) {
	for(uint i=0;i<num;i++)
		carray[i]=getNewVarSATEncoder(encoder);
}

Edge getNewVarSATEncoder(SATEncoder *This) {
	return constraintNewVar(This->cnf);
}

Edge encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint) {
	if (edgeIsNull(constraint->var)) {
		constraint->var=getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Edge encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint) {
	Edge array[getSizeArrayBoolean(&constraint->inputs)];
	for(uint i=0;i<getSizeArrayBoolean(&constraint->inputs);i++)
		array[i]=encodeConstraintSATEncoder(This, getArrayBoolean(&constraint->inputs, i));

	switch(constraint->op) {
	case L_AND:
		return constraintAND(This->cnf, getSizeArrayBoolean(&constraint->inputs), array);
	case L_OR:
		return constraintOR(This->cnf, getSizeArrayBoolean(&constraint->inputs), array);
	case L_NOT:
		ASSERT( getSizeArrayBoolean(&constraint->inputs)==1);
		return constraintNegate(array[0]);
	case L_XOR:
		ASSERT( getSizeArrayBoolean(&constraint->inputs)==2);
		return constraintXOR(This->cnf, array[0], array[1]);
	case L_IMPLIES:
		ASSERT( getSizeArrayBoolean( &constraint->inputs)==2);
		return constraintIMPLIES(This->cnf, array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}


Edge encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint) {
	switch( constraint->order->type){
		case PARTIAL:
			return encodePartialOrderSATEncoder(This, constraint);
		case TOTAL:
			return encodeTotalOrderSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge getPairConstraint(SATEncoder *This, HashTableBoolConst * table, OrderPair * pair) {
	bool negate = false;
	OrderPair flipped;
	if (pair->first > pair->second) {
		negate=true;
		flipped.first=pair->second;
		flipped.second=pair->first;
		pair = &flipped;	//FIXME: accessing a local variable from outside of the function?
	}
	Edge constraint;
	if (!containsBoolConst(table, pair)) {
		constraint = getNewVarSATEncoder(This);
		OrderPair * paircopy = allocOrderPair(pair->first, pair->second, constraint);
		putBoolConst(table, paircopy, paircopy);
	} else
		constraint = getBoolConst(table, pair)->constraint;
	if (negate)
		return constraintNegate(constraint);
	else
		return constraint;
	
}

Edge encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * boolOrder){
	ASSERT(boolOrder->order->type == TOTAL);
	if(boolOrder->order->boolsToConstraints == NULL){
		initializeOrderHashTable(boolOrder->order);
		createAllTotalOrderConstraintsSATEncoder(This, boolOrder->order);
	}
	HashTableBoolConst* boolToConsts = boolOrder->order->boolsToConstraints;
	OrderPair pair={boolOrder->first, boolOrder->second, E_NULL};
	Edge constraint = getPairConstraint(This, boolToConsts, & pair);
	return constraint;
}

void createAllTotalOrderConstraintsSATEncoder(SATEncoder* This, Order* order){
	ASSERT(order->type == TOTAL);
	VectorInt* mems = order->set->members;
	HashTableBoolConst* table = order->boolsToConstraints;
	uint size = getSizeVectorInt(mems);
	uint csize =0;
	for(uint i=0; i<size; i++){
		uint64_t valueI = getVectorInt(mems, i);
		for(uint j=i+1; j<size;j++){
			uint64_t valueJ = getVectorInt(mems, j);
			OrderPair pairIJ = {valueI, valueJ};
			Edge constIJ=getPairConstraint(This, table, & pairIJ);
			for(uint k=j+1; k<size; k++){
				uint64_t valueK = getVectorInt(mems, k);
				OrderPair pairJK = {valueJ, valueK};
				OrderPair pairIK = {valueI, valueK};
				Edge constIK = getPairConstraint(This, table, & pairIK);
				Edge constJK = getPairConstraint(This, table, & pairJK);
				addConstraint(This->cnf, generateTransOrderConstraintSATEncoder(This, constIJ, constJK, constIK)); 
			}
		}
	}
}

Edge getOrderConstraint(HashTableBoolConst *table, OrderPair *pair){
	ASSERT(pair->first!= pair->second);
	Edge constraint = getBoolConst(table, pair)->constraint;
	if(pair->first > pair->second)
		return constraint;
	else
		return constraintNegate(constraint);
}

Edge generateTransOrderConstraintSATEncoder(SATEncoder *This, Edge constIJ,Edge constJK,Edge constIK){
	Edge carray[] = {constIJ, constJK, constraintNegate(constIK)};
	Edge loop1= constraintOR(This->cnf, 3, carray);
	Edge carray2[] = {constraintNegate(constIJ), constraintNegate(constJK), constIK};
	Edge loop2= constraintOR(This->cnf, 3, carray2 );
	return constraintAND2(This->cnf, loop1, loop2);
}

Edge encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint){
	// FIXME: we can have this implementation for partial order. Basically,
	// we compute the transitivity between two order constraints specified by the client! (also can be used
	// when client specify sparse constraints for the total order!)
	ASSERT(constraint->order->type == PARTIAL);
/*
	HashTableBoolConst* boolToConsts = boolOrder->order->boolsToConstraints;
	if( containsBoolConst(boolToConsts, boolOrder) ){
		return getBoolConst(boolToConsts, boolOrder);
	} else {
		Edge constraint = getNewVarSATEncoder(This); 
		putBoolConst(boolToConsts,boolOrder, constraint);
		VectorBoolean* orderConstrs = &boolOrder->order->constraints;
		uint size= getSizeVectorBoolean(orderConstrs);
		for(uint i=0; i<size; i++){
			ASSERT(GETBOOLEANTYPE( getVectorBoolean(orderConstrs, i)) == ORDERCONST );
			BooleanOrder* tmp = (BooleanOrder*)getVectorBoolean(orderConstrs, i);
			BooleanOrder* newBool;
			Edge first, second;
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
			Edge transConstr= encodeTotalOrderSATEncoder(This, newBool);
			generateTransOrderConstraintSATEncoder(This, first, second, transConstr );
		}
		return constraint;
	}
*/	
	return E_BOGUS;
}

Edge encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	switch(GETPREDICATETYPE(constraint->predicate) ){
		case TABLEPRED:
			return encodeTablePredicateSATEncoder(This, constraint);
		case OPERATORPRED:
			return encodeOperatorPredicateSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge encodeTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
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
	return E_BOGUS;
}

Edge encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	VectorTableEntry* entries = &(((PredicateTable*)constraint->predicate)->table->entries);
	FunctionEncodingType encType = constraint->encoding.type;
	ArrayElement* inputs = &constraint->inputs;
	uint inputNum =getSizeArrayElement(inputs);
	//Encode all the inputs first ...
	for(uint i=0; i<inputNum; i++){
		encodeElementSATEncoder(This, getArrayElement(inputs, i));
	}
	
	uint size = getSizeVectorTableEntry(entries);
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
	Edge constraints[size];
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(entries, i);
		if(generateNegation == entry->output) {
			//Skip the irrelevant entries
			continue;
		}
		Edge carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el = getArrayElement(inputs, j);
			carray[j] = getElementValueConstraint(This, el, entry->inputs[j]);
		}
		constraints[i]=constraintAND(This->cnf, inputNum, carray);
	}
	Edge result=constraintOR(This->cnf, size, constraints);

	return generateNegation ? result: constraintNegate(result);
}

Edge encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	switch(constraint->encoding.type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumOperatorPredicateSATEncoder(This, constraint);
		case CIRCUIT:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	ASSERT(GETPREDICATETYPE(constraint->predicate)==OPERATORPRED);
	PredicateOperator* predicate = (PredicateOperator*)constraint->predicate;
	ASSERT(predicate->op == EQUALS); //For now, we just only support equals
	//getting maximum size of in common elements between two sets!
	uint size=getSizeVectorInt( getArraySet( &predicate->domains, 0)->members);
	uint64_t commonElements [size];
	getEqualitySetIntersection(predicate, &size, commonElements);
	Edge  carray[size];
	Element* elem1 = getArrayElement( &constraint->inputs, 0);
	Element* elem2 = getArrayElement( &constraint->inputs, 1);
	encodeElementSATEncoder(This,elem1);
	encodeElementSATEncoder(This, elem2);
	
	for(uint i=0; i<size; i++){
		Edge arg1 = getElementValueConstraint(This, elem1, commonElements[i]);
		Edge arg2 = getElementValueConstraint(This, elem2, commonElements[i]);
		carray[i] =  constraintAND2(This->cnf, arg1, arg2);
	}
	//FIXME: the case when there is no intersection ....
	return constraintOR(This->cnf, size, carray);
}

void encodeElementSATEncoder(SATEncoder* encoder, Element *This){
	switch( GETELEMENTTYPE(This) ){
		case ELEMFUNCRETURN:
			Edge c = encodeFunctionElementSATEncoder(This, (ElementFunction*) This);
			addConstraint(encoder->cnf, c);
			break;
		case ELEMSET:
			return;
		default:
			ASSERT(0);
	}
}

Edge encodeFunctionElementSATEncoder(SATEncoder* encoder, ElementFunction *This){
	switch(GETFUNCTIONTYPE(This->function)){
		case TABLEFUNC:
			return encodeTableElementFunctionSATEncoder(encoder, This);
		case OPERATORFUNC:
			return encodeOperatorElementFunctionSATEncoder(encoder, This);
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
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
	return E_BOGUS;
}

Edge encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function) == OPERATORFUNC);
	ASSERT(getSizeArrayElement(&This->inputs)==2 );
	Element* elem1 = getArrayElement(&This->inputs,0);
	Element* elem2 = getArrayElement(&This->inputs,1);
	encodeElementSATEncoder(encoder, elem1);
	encodeElementSATEncoder(encoder , elem2);
	
	ElementEncoding* elemEnc1 = getElementEncoding( getArrayElement(&This->inputs,0) );
	ElementEncoding* elemEnc2 = getElementEncoding( getArrayElement(&This->inputs,1) );
	Edge carray[elemEnc1->encArraySize*elemEnc2->encArraySize];
	uint size=0;
	Edge overFlowConstraint = ((BooleanVar*) This->overflowstatus)->var;
	for(uint i=0; i<elemEnc1->encArraySize; i++){
		if(isinUseElement(elemEnc1, i)){
			for( uint j=0; j<elemEnc2->encArraySize; j++){
				if(isinUseElement(elemEnc2, j)){
					bool isInRange = false;
					uint64_t result= applyFunctionOperator((FunctionOperator*)This->function,elemEnc1->encodingArray[i],
						elemEnc2->encodingArray[j], &isInRange);
					//FIXME: instead of getElementValueConstraint, it might be useful to have another function
					// that doesn't iterate over encodingArray and treats more efficient ...
					Edge valConstrIn1 = getElementValueConstraint(encoder, elemEnc1->element, elemEnc1->encodingArray[i]);
					Edge valConstrIn2 = getElementValueConstraint(encoder, elemEnc2->element, elemEnc2->encodingArray[j]);
					Edge valConstrOut = getElementValueConstraint(encoder, (Element*) This, result);
					if(edgeIsNull(valConstrOut))
						continue; //FIXME:Should talk to brian about it!
					Edge OpConstraint = constraintIMPLIES(encoder->cnf, constraintAND2(encoder->cnf, valConstrIn1, valConstrIn2), valConstrOut);
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
								Edge const1 = constraintIMPLIES(encoder->cnf, constraintAND2(encoder->cnf, valConstrIn1, valConstrIn2), constraintNegate(overFlowConstraint));
								carray[size++] = constraintAND2(encoder->cnf, const1, OpConstraint);
							}
							break;
						case OVERFLOWSETSFLAG:
							if(isInRange){
								carray[size++] = OpConstraint;
							} else{
								carray[size++] = constraintIMPLIES(encoder->cnf, constraintAND2(encoder->cnf, valConstrIn1, valConstrIn2), overFlowConstraint);
							}
							break;
						case FLAGIFFOVERFLOW:
							if(isInRange){
								Edge const1 = constraintIMPLIES(encoder->cnf, constraintAND2(encoder->cnf, valConstrIn1, valConstrIn2), constraintNegate(overFlowConstraint));
								carray[size++] = constraintAND2(encoder->cnf, const1, OpConstraint);
							} else {
								carray[size++] = constraintIMPLIES(encoder->cnf, constraintAND2(encoder->cnf, valConstrIn1, valConstrIn2), overFlowConstraint);
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
	Edge result = constraintAND(encoder->cnf, size, carray); 
	if (!edgeIsNull(elemc1))
		result = constraintAND2(encoder->cnf, result, elemc1);
	if (!edgeIsNull(elemc2))
		result = constraintAND2(encoder->cnf, result, elemc2);
	return result;
}

Edge encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function)==TABLEFUNC);
	ArrayElement* elements= &This->inputs;
	Table* table = ((FunctionTable*) (This->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	Edge constraints[size]; //FIXME: should add a space for the case that didn't match any entries
	for(uint i=0; i<size; i++) {
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		uint inputNum = getSizeArrayElement(elements);
		Edge carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el= getArrayElement(elements, j);
			carray[j] = getElementValueConstraint(encoder, el, entry->inputs[j]);
		}
		Edge output = getElementValueConstraint(encoder, (Element*)This, entry->output);
		Edge row= constraintIMPLIES(encoder->cnf, constraintAND(encoder->cnf, inputNum, carray), output);
		constraints[i]=row;
	}
	return constraintOR(encoder->cnf, size, constraints);
}
