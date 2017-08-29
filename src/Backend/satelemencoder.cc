#include "satencoder.h"
#include "structs.h"
#include "common.h"
#include "ops.h"
#include "element.h"
#include "set.h"

Edge SATEncoder::getElementValueConstraint(Element *elem, uint64_t value) {
	switch (getElementEncoding(elem)->type) {
	case ONEHOT:
		return getElementValueOneHotConstraint(elem, value);
	case UNARY:
		return getElementValueUnaryConstraint(elem, value);
	case BINARYINDEX:
		return getElementValueBinaryIndexConstraint(elem, value);
	case ONEHOTBINARY:
		ASSERT(0);
		break;
	case BINARYVAL:
		return getElementValueBinaryValueConstraint(elem, value);
		break;
	default:
		ASSERT(0);
		break;
	}
	return E_BOGUS;
}

Edge SATEncoder::getElementValueBinaryIndexConstraint(Element *elem, uint64_t value) {
	ASTNodeType type = elem->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN || type == ELEMCONST);
	ElementEncoding *elemEnc = getElementEncoding(elem);
	for (uint i = 0; i < elemEnc->encArraySize; i++) {
		if (elemEnc->isinUseElement(i) && elemEnc->encodingArray[i] == value) {
			return (elemEnc->numVars == 0) ? E_True : generateBinaryConstraint(cnf, elemEnc->numVars, elemEnc->variables, i);
		}
	}
	return E_False;
}

Edge SATEncoder::getElementValueOneHotConstraint(Element *elem, uint64_t value) {
	ASTNodeType type = elem->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN || type == ELEMCONST);
	ElementEncoding *elemEnc = getElementEncoding(elem);
	for (uint i = 0; i < elemEnc->encArraySize; i++) {
		if (elemEnc->isinUseElement(i) && elemEnc->encodingArray[i] == value) {
			return (elemEnc->numVars == 0) ? E_True : elemEnc->variables[i];
		}
	}
	return E_False;
}

Edge SATEncoder::getElementValueUnaryConstraint(Element *elem, uint64_t value) {
	ASTNodeType type = elem->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN || type == ELEMCONST);
	ElementEncoding *elemEnc = getElementEncoding(elem);
	for (uint i = 0; i < elemEnc->encArraySize; i++) {
		if (elemEnc->isinUseElement(i) && elemEnc->encodingArray[i] == value) {
			if (elemEnc->numVars == 0)
				return E_True;
			if (i == 0)
				return constraintNegate(elemEnc->variables[0]);
			else if ((i + 1) == elemEnc->encArraySize)
				return elemEnc->variables[i - 1];
			else
				return constraintAND2(cnf, elemEnc->variables[i - 1], constraintNegate(elemEnc->variables[i]));
		}
	}
	return E_False;
}

Edge SATEncoder::getElementValueBinaryValueConstraint(Element *element, uint64_t value) {
	ASTNodeType type = element->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding *elemEnc = getElementEncoding(element);
	if (elemEnc->low <= elemEnc->high) {
		if (value < elemEnc->low || value > elemEnc->high)
			return E_False;
	} else {
		//Range wraps around 0
		if (value < elemEnc->low && value > elemEnc->high)
			return E_False;
	}

	uint64_t valueminusoffset = value - elemEnc->offset;
	return generateBinaryConstraint(cnf, elemEnc->numVars, elemEnc->variables, valueminusoffset);
}

void allocElementConstraintVariables(ElementEncoding *This, uint numVars) {
	This->numVars = numVars;
	This->variables = (Edge *)ourmalloc(sizeof(Edge) * numVars);
}

void SATEncoder::generateBinaryValueEncodingVars(ElementEncoding *encoding) {
	ASSERT(encoding->type == BINARYVAL);
	allocElementConstraintVariables(encoding, encoding->numBits);
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
}

void SATEncoder::generateBinaryIndexEncodingVars(ElementEncoding *encoding) {
	ASSERT(encoding->type == BINARYINDEX);
	allocElementConstraintVariables(encoding, NUMBITS(encoding->encArraySize - 1));
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
}

void SATEncoder::generateOneHotEncodingVars(ElementEncoding *encoding) {
	allocElementConstraintVariables(encoding, encoding->encArraySize);
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
	for (uint i = 0; i < encoding->numVars; i++) {
		for (uint j = i + 1; j < encoding->numVars; j++) {
			addConstraintCNF(cnf, constraintNegate(constraintAND2(cnf, encoding->variables[i], encoding->variables[j])));
		}
	}
	addConstraintCNF(cnf, constraintOR(cnf, encoding->numVars, encoding->variables));
}

void SATEncoder::generateUnaryEncodingVars(ElementEncoding *encoding) {
	allocElementConstraintVariables(encoding, encoding->encArraySize - 1);
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
	//Add unary constraint
	for (uint i = 1; i < encoding->numVars; i++) {
		addConstraintCNF(cnf, constraintOR2(cnf, encoding->variables[i - 1], constraintNegate(encoding->variables[i])));
	}
}

void SATEncoder::generateElementEncoding(Element *element) {
	ElementEncoding *encoding = getElementEncoding(element);
	ASSERT(encoding->type != ELEM_UNASSIGNED);
	if (encoding->variables != NULL)
		return;
	switch (encoding->type) {
	case ONEHOT:
		generateOneHotEncodingVars(encoding);
		return;
	case BINARYINDEX:
		generateBinaryIndexEncodingVars(encoding);
		return;
	case UNARY:
		generateUnaryEncodingVars(encoding);
		return;
	case ONEHOTBINARY:
		return;
	case BINARYVAL:
		generateBinaryValueEncodingVars(encoding);
		return;
	default:
		ASSERT(0);
	}
}

