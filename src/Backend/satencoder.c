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
		addConstraintCNF(This->cnf, c);
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

Edge getPairConstraint(SATEncoder *This, HashTableOrderPair * table, OrderPair * pair) {
	bool negate = false;
	OrderPair flipped;
	if (pair->first > pair->second) {
		negate=true;
		flipped.first=pair->second;
		flipped.second=pair->first;
		pair = &flipped;
	}
	Edge constraint;
	if (!containsOrderPair(table, pair)) {
		constraint = getNewVarSATEncoder(This);
		OrderPair * paircopy = allocOrderPair(pair->first, pair->second, constraint);
		putOrderPair(table, paircopy, paircopy);
	} else
		constraint = getOrderPair(table, pair)->constraint;

	return negate ? constraintNegate(constraint) : constraint;
}

Edge encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * boolOrder) {
	ASSERT(boolOrder->order->type == TOTAL);
	if(boolOrder->order->orderPairTable == NULL) {
		initializeOrderHashTable(boolOrder->order);
		createAllTotalOrderConstraintsSATEncoder(This, boolOrder->order);
	}
	HashTableOrderPair* orderPairTable = boolOrder->order->orderPairTable;
	OrderPair pair={boolOrder->first, boolOrder->second, E_NULL};
	Edge constraint = getPairConstraint(This, orderPairTable, & pair);
	return constraint;
}

void createAllTotalOrderConstraintsSATEncoder(SATEncoder* This, Order* order){
	ASSERT(order->type == TOTAL);
	VectorInt* mems = order->set->members;
	HashTableOrderPair* table = order->orderPairTable;
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
				addConstraintCNF(This->cnf, generateTransOrderConstraintSATEncoder(This, constIJ, constJK, constIK)); 
			}
		}
	}
}

Edge getOrderConstraint(HashTableOrderPair *table, OrderPair *pair){
	ASSERT(pair->first!= pair->second);
	Edge constraint = getOrderPair(table, pair)->constraint;
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
	ASSERT(constraint->order->type == PARTIAL);
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
	
	//WARNING: THIS ASSUMES PREDICATE TABLE IS COMPLETE...SEEMS UNLIKELY TO BE SAFE IN MANY CASES...
	//WONDER WHAT BEST WAY TO HANDLE THIS IS...
	
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

Edge encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
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

Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	PredicateOperator* predicate = (PredicateOperator*)constraint->predicate;
	uint numDomains=getSizeArraySet(&predicate->domains);

	FunctionEncodingType encType = constraint->encoding.type;
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;

	/* Call base encoders for children */
	for(uint i=0;i<numDomains;i++) {
		Element *elem = getArrayElement( &constraint->inputs, i);
		encodeElementSATEncoder(This, elem);
	}
	VectorEdge * clauses=allocDefVectorEdge(); // Setup array of clauses
	
	uint indices[numDomains]; //setup indices
	bzero(indices, sizeof(uint)*numDomains);
	
	uint64_t vals[numDomains]; //setup value array
	for(uint i=0;i<numDomains; i++) {
		Set * set=getArraySet(&predicate->domains, i);
		vals[i]=getSetElement(set, indices[i]);
	}
	
	bool notfinished=true;
	while(notfinished) {
		Edge carray[numDomains];

		if (evalPredicateOperator(predicate, vals) ^ generateNegation) {
			//Include this in the set of terms
			for(uint i=0;i<numDomains;i++) {
				Element * elem = getArrayElement(&constraint->inputs, i);
				carray[i] = getElementValueConstraint(This, elem, vals[i]);
			}
			pushVectorEdge(clauses, constraintAND(This->cnf, numDomains, carray));
		}
		
		notfinished=false;
		for(uint i=0;i<numDomains; i++) {
			uint index=++indices[i];
			Set * set=getArraySet(&predicate->domains, i);

			if (index < getSetSize(set)) {
				vals[i]=getSetElement(set, index);
				notfinished=true;
				break;
			} else {
				indices[i]=0;
				vals[i]=getSetElement(set, 0);
			}
		}
	}

	Edge cor=constraintOR(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	deleteVectorEdge(clauses);
	return generateNegation ? cor : constraintNegate(cor);
}

void encodeElementSATEncoder(SATEncoder* encoder, Element *This){
	switch( GETELEMENTTYPE(This) ){
		case ELEMFUNCRETURN:
			addConstraintCNF(encoder->cnf, encodeElementFunctionSATEncoder(encoder, (ElementFunction*) This));
			break;
		case ELEMSET:
			return;
		default:
			ASSERT(0);
	}
}

Edge encodeElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction *This){
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

Edge encodeOperatorElementFunctionSATEncoder(SATEncoder* This, ElementFunction* func) {
	FunctionOperator * function = (FunctionOperator *) func->function;
	uint numDomains=getSizeArrayElement(&func->inputs);

	/* Call base encoders for children */
	for(uint i=0;i<numDomains;i++) {
		Element *elem = getArrayElement( &func->inputs, i);
		encodeElementSATEncoder(This, elem);
	}

	VectorEdge * clauses=allocDefVectorEdge(); // Setup array of clauses
	
	uint indices[numDomains]; //setup indices
	bzero(indices, sizeof(uint)*numDomains);
	
	uint64_t vals[numDomains]; //setup value array
	for(uint i=0;i<numDomains; i++) {
		Set * set=getElementSet(getArrayElement(&func->inputs, i));
		vals[i]=getSetElement(set, indices[i]);
	}

	Edge overFlowConstraint = ((BooleanVar*) func->overflowstatus)->var;
	
	bool notfinished=true;
	while(notfinished) {
		Edge carray[numDomains+2];

		uint64_t result=applyFunctionOperator(function, numDomains, vals);
		bool isInRange = isInRangeFunction((FunctionOperator*)func->function, result);
		bool needClause = isInRange;
		if (function->overflowbehavior == OVERFLOWSETSFLAG || function->overflowbehavior == FLAGIFFOVERFLOW) {
			needClause=true;
		}
		
		if (needClause) {
			//Include this in the set of terms
			for(uint i=0;i<numDomains;i++) {
				Element * elem = getArrayElement(&func->inputs, i);
				carray[i] = getElementValueConstraint(This, elem, vals[i]);
			}
			if (isInRange) {
				carray[numDomains] = getElementValueConstraint(This, &func->base, result);
			}

			Edge clause;
			switch(function->overflowbehavior) {
			case IGNORE:
			case NOOVERFLOW:
			case WRAPAROUND: {
				clause=constraintAND(This->cnf, numDomains+1, carray);
				break;
			}
			case FLAGFORCESOVERFLOW: {
				carray[numDomains+1]=constraintNegate(overFlowConstraint);
				clause=constraintAND(This->cnf, numDomains+2, carray);
				break;
			}
			case OVERFLOWSETSFLAG: {
				if (isInRange) {
					clause=constraintAND(This->cnf, numDomains+1, carray);
				} else {
					carray[numDomains+1]=overFlowConstraint;
					clause=constraintAND(This->cnf, numDomains+1, carray);
				}
				break;
			}
			case FLAGIFFOVERFLOW: {
				if (isInRange) {
				carray[numDomains+1]=constraintNegate(overFlowConstraint);
					clause=constraintAND(This->cnf, numDomains+2, carray);
				} else {
					carray[numDomains+1]=overFlowConstraint;
					clause=constraintAND(This->cnf, numDomains+1, carray);
				}
				break;
			}
			default:
				ASSERT(0);
			}
			pushVectorEdge(clauses, clause);
		}
		
		notfinished=false;
		for(uint i=0;i<numDomains; i++) {
			uint index=++indices[i];
			Set * set=getElementSet(getArrayElement(&func->inputs, i));

			if (index < getSetSize(set)) {
				vals[i]=getSetElement(set, index);
				notfinished=true;
				break;
			} else {
				indices[i]=0;
				vals[i]=getSetElement(set, 0);
			}
		}
	}

	Edge cor=constraintOR(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	deleteVectorEdge(clauses);
	return cor;
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
