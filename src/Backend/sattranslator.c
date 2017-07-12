#include "sattranslator.h"
#include "element.h"
#include "csolver.h"
#include "satencoder.h"

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
uint64_t getElementValueOneHotSATTranslator(CSolver* This, ElementEncoding* elemEnc){
	uint index=0;
	for(int i=elemEnc->numVars-1;i>=0;i--) {
		if (This->satEncoder->cnf->solver->solution[ getEdgeVar( elemEnc->variables[i] ) ])
			index = i;
	}
	ASSERT(elemEnc->encArraySize >index && isinUseElement(elemEnc, index));
	return elemEnc->encodingArray[index];
}
uint64_t getElementValueSATTranslator(CSolver* This, Element* element){
	switch(getElementEncoding(element)->type){
		case ONEHOT:
			ASSERT(0);
			break;
		case UNARY:
			ASSERT(0);
			break;
		case BINARYINDEX:
			return getElementValueBinaryIndexSATTranslator(This, getElementEncoding(element));
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
}

bool getBooleanVariableValueSATTranslator( CSolver* This , Boolean* boolean){
	int index = getEdgeVar( ((BooleanVar*) boolean)->var );
	return This->satEncoder->cnf->solver->solution[index] == true;
}