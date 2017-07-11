#include "satencoder.h"
#include "structs.h"
#include "common.h"
#include "ops.h"
#include "element.h"

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

