#include "satencoder.h"
#include "structs.h"
#include "common.h"
#include "ops.h"
#include "element.h"

Edge getElementValueConstraint(SATEncoder* This, Element* elem, uint64_t value) {
	switch(getElementEncoding(elem)->type){
		case ONEHOT:
			return getElementValueOneHotConstraint(This, elem, value);
		case UNARY:
			ASSERT(0);
			break;
		case BINARYINDEX:
			return getElementValueBinaryIndexConstraint(This, elem, value);
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
		if(isinUseElement(elemEnc, i) && elemEnc->encodingArray[i]==value) {
			return generateBinaryConstraint(This->cnf, elemEnc->numVars, elemEnc->variables, i);
		}
	}
	return E_BOGUS;
}

Edge getElementValueOneHotConstraint(SATEncoder * This, Element* elem, uint64_t value) {
	ASTNodeType type = GETELEMENTTYPE(elem);
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding* elemEnc = getElementEncoding(elem);
	for(uint i=0; i<elemEnc->encArraySize; i++){
		if (isinUseElement(elemEnc, i) && elemEnc->encodingArray[i]==value) {
			return elemEnc->variables[i];
		}
	}
	return E_BOGUS;
}

void allocElementConstraintVariables(ElementEncoding* This, uint numVars) {
	This->numVars = numVars;
	This->variables = ourmalloc(sizeof(Edge) * numVars);
}

void generateBinaryIndexEncodingVars(SATEncoder *This, ElementEncoding *encoding) {
	ASSERT(encoding->type==BINARYINDEX);
	allocElementConstraintVariables(encoding, NUMBITS(encoding->encArraySize-1));
	getArrayNewVarsSATEncoder(This, encoding->numVars, encoding->variables);
}

void generateOneHotEncodingVars(SATEncoder *This, ElementEncoding *encoding) {
	allocElementConstraintVariables(encoding, encoding->encArraySize);
	getArrayNewVarsSATEncoder(This, encoding->numVars, encoding->variables);	
	for(uint i=0;i<encoding->numVars;i++) {
		for(uint j=0;j<encoding->numVars;j++) {
			addConstraintCNF(This->cnf, constraintIMPLIES(This->cnf, encoding->variables[i], constraintNegate(encoding->variables[j])));
		}
	}
}

void generateElementEncoding(SATEncoder* This, Element * element) {
	ElementEncoding* encoding = getElementEncoding(element);
	ASSERT(encoding->type!=ELEM_UNASSIGNED);
	if(encoding->variables!=NULL)
		return;
	switch(encoding->type) {
	case ONEHOT:
		generateOneHotEncodingVars(This, encoding);
		return;
	case BINARYINDEX:
		generateBinaryIndexEncodingVars(This, encoding);
		return;
	default:
		ASSERT(0);
	}
}

