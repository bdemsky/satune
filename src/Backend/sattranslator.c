#include "sattranslator.h"
#include "element.h"
#include "csolver.h"
#include "satencoder.h"
#include "set.h"
#include "order.h"
#include "orderpair.h"

uint64_t getElementValueBinaryIndexSATTranslator(CSolver* This, ElementEncoding* elemEnc){
	uint index=0;
	for(int i=elemEnc->numVars-1;i>=0;i--) {
		index=index<<1;
		if (This->satEncoder->cnf->solver->solution[ getEdgeVar( elemEnc->variables[i] ) ])
			index |= 1;
	}
	ASSERT(elemEnc->encArraySize >index && isinUseElement(elemEnc, index));
	return elemEnc->encodingArray[index];
}

uint64_t getElementValueBinaryValueSATTranslator(CSolver* This, ElementEncoding* elemEnc){
	uint64_t value=0;
	for(int i=elemEnc->numVars-1;i>=0;i--) {
		value=value<<1;
		if (This->satEncoder->cnf->solver->solution[ getEdgeVar( elemEnc->variables[i] ) ])
			value |= 1;
	}
	if (elemEnc->isBinaryValSigned &&
			This->satEncoder->cnf->solver->solution[ getEdgeVar( elemEnc->variables[elemEnc->numVars-1])]) {
		//Do sign extension of negative number
		uint64_t highbits=0xffffffffffffffff - ((1 << (elemEnc->numVars)) - 1);
		value+=highbits;
	}
	value+=elemEnc->offset;
	return value;
}

uint64_t getElementValueOneHotSATTranslator(CSolver* This, ElementEncoding* elemEnc){
	uint index=0;
	for(int i=elemEnc->numVars-1;i>=0;i--) {
		if (This->satEncoder->cnf->solver->solution[ getEdgeVar( elemEnc->variables[i] ) ])
			index = i;
	}
	ASSERT(elemEnc->encArraySize >index && isinUseElement(elemEnc, index));
	return elemEnc->encodingArray[index];
}

uint64_t getElementValueUnarySATTranslator(CSolver* This, ElementEncoding* elemEnc){
	uint i;
	for(i=0;i<elemEnc->numVars;i++) {
		if (!This->satEncoder->cnf->solver->solution[ getEdgeVar( elemEnc->variables[i] ) ]) {
			break;
		}
	}

	return elemEnc->encodingArray[i];
}

uint64_t getElementValueSATTranslator(CSolver* This, Element* element){
	ElementEncoding* elemEnc = getElementEncoding(element);
	if(elemEnc->numVars == 0)	//case when the set has only one item
		return getSetElement(getElementSet(element), 0);
	switch(elemEnc->type){
		case ONEHOT:
			getElementValueOneHotSATTranslator(This, elemEnc);
			break;
		case UNARY:
			getElementValueUnarySATTranslator(This, elemEnc);
			break;
		case BINARYINDEX:
			return getElementValueBinaryIndexSATTranslator(This, elemEnc);
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
	return -1;
}

bool getBooleanVariableValueSATTranslator( CSolver* This , Boolean* boolean){
	int index = getEdgeVar( ((BooleanVar*) boolean)->var );
	return This->satEncoder->cnf->solver->solution[index] == true;
}

HappenedBefore getOrderConstraintValueSATTranslator(CSolver* This, BooleanOrder* boolOrder){
	ASSERT(boolOrder->order->orderPairTable!= NULL);
	OrderPair pair={boolOrder->first, boolOrder->second, E_NULL};
	Edge var = getOrderConstraint(boolOrder->order->orderPairTable, & pair);
	if(edgeIsNull(var))
		return UNORDERED;
	int index = getEdgeVar( var );
	return This->satEncoder->cnf->solver->solution[index] ? FIRST: SECOND;
}

