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
			return getElementValueUnaryConstraint(This, elem, value);
		case BINARYINDEX:
			return getElementValueBinaryIndexConstraint(This, elem, value);
		case ONEHOTBINARY:
			ASSERT(0);
			break;
		case BINARYVAL:
			return getElementValueBinaryValueConstraint(This, elem, value);
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
			return (elemEnc->numVars == 0) ? E_True: generateBinaryConstraint(This->cnf, elemEnc->numVars, elemEnc->variables, i);
		}
	}
	return E_False;
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

Edge getElementValueUnaryConstraint(SATEncoder * This, Element* elem, uint64_t value) {
	ASTNodeType type = GETELEMENTTYPE(elem);
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding* elemEnc = getElementEncoding(elem);
	for(uint i=0; i<elemEnc->encArraySize; i++){
		if (isinUseElement(elemEnc, i) && elemEnc->encodingArray[i]==value) {
			if (i==0)
				return constraintNegate(elemEnc->variables[0]);
			else if ((i+1)==elemEnc->encArraySize)
				return elemEnc->variables[i-1];
			else
				return constraintAND2(This->cnf, elemEnc->variables[i-1], constraintNegate(elemEnc->variables[i]));
		}
	}
	return E_BOGUS;
}

Edge getElementValueBinaryValueConstraint(SATEncoder * This, Element* element, uint64_t value){
	ASTNodeType type = GETELEMENTTYPE(element);
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding* elemEnc = getElementEncoding(element);
	if (elemEnc->low <= elemEnc->high) {
		if (value < elemEnc->low || value > elemEnc->high)
			return E_False;
	} else {
		//Range wraps around 0
		if (value < elemEnc->low && value > elemEnc->high)
			return E_False;
	}
	
	uint64_t valueminusoffset=value-elemEnc->offset;
	return generateBinaryConstraint(This->cnf, elemEnc->numVars, elemEnc->variables, valueminusoffset);
}

void allocElementConstraintVariables(ElementEncoding* This, uint numVars) {
	This->numVars = numVars;
	This->variables = ourmalloc(sizeof(Edge) * numVars);
}

void generateBinaryValueEncodingVars(SATEncoder* This, ElementEncoding* encoding){
	ASSERT(encoding->type==BINARYVAL);
	allocElementConstraintVariables(encoding, encoding->numBits);
	getArrayNewVarsSATEncoder(This, encoding->numVars, encoding->variables);
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
		for(uint j=i+1;j<encoding->numVars;j++) {
			addConstraintCNF(This->cnf, constraintNegate(constraintAND2(This->cnf, encoding->variables[i], encoding->variables[j])));
		}
	}
	addConstraintCNF(This->cnf, constraintOR(This->cnf, encoding->numVars, encoding->variables));
}

void generateUnaryEncodingVars(SATEncoder *This, ElementEncoding *encoding) {
	allocElementConstraintVariables(encoding, encoding->encArraySize-1);
	getArrayNewVarsSATEncoder(This, encoding->numVars, encoding->variables);	
	//Add unary constraint
	for(uint i=1;i<encoding->numVars;i++) {
		addConstraintCNF(This->cnf, constraintOR2(This->cnf, encoding->variables[i-1], constraintNegate(encoding->variables[i])));
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
	case UNARY:
		generateUnaryEncodingVars(This, encoding);
		return;
	case ONEHOTBINARY:
		return;
	case BINARYVAL:
		generateBinaryValueEncodingVars(This, encoding);
		return;
	default:
		ASSERT(0);
	}
}

