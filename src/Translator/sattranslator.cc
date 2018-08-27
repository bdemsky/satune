#include "sattranslator.h"
#include "element.h"
#include "csolver.h"
#include "satencoder.h"
#include "set.h"
#include "order.h"
#include "orderpair.h"

uint64_t getElementValueBinaryIndexSATTranslator(CSolver *This, ElementEncoding *elemEnc) {
	uint index = 0;
	for (int i = elemEnc->numVars - 1; i >= 0; i--) {
		index = index << 1;
		if (getValueSolver(This->getSATEncoder()->getCNF()->solver, getEdgeVar( elemEnc->variables[i] )))
			index |= 1;
	}
	if (elemEnc->encArraySize <= index || !elemEnc->isinUseElement(index))
		model_print("WARNING: Element has undefined value!\n");
	return elemEnc->encodingArray[index];
}

uint64_t getElementValueBinaryValueSATTranslator(CSolver *This, ElementEncoding *elemEnc) {
	uint64_t value = 0;
	for (int i = elemEnc->numVars - 1; i >= 0; i--) {
		value = value << 1;
		if (getValueSolver(This->getSATEncoder()->getCNF()->solver, getEdgeVar( elemEnc->variables[i] )) )
			value |= 1;
	}
	if (elemEnc->isBinaryValSigned &&
			This->getSATEncoder()->getCNF()->solver->solution[ getEdgeVar( elemEnc->variables[elemEnc->numVars - 1])]) {
		//Do sign extension of negative number
		uint64_t highbits = 0xffffffffffffffff - ((1 << (elemEnc->numVars)) - 1);
		value += highbits;
	}
	value += elemEnc->offset;
	return value;
}

uint64_t getElementValueOneHotSATTranslator(CSolver *This, ElementEncoding *elemEnc) {
	uint index = 0;
	bool overflow = true;
	for (uint i = 0; i < elemEnc->numVars; i++) {
		if (getValueSolver(This->getSATEncoder()->getCNF()->solver, getEdgeVar( elemEnc->variables[i] ))) {
			index = i;
			overflow = false;
		}
	}
	if (overflow)
		model_print("WARNING: Element has undefined value!\n");
	ASSERT(elemEnc->encArraySize > index && elemEnc->isinUseElement(index));
	return elemEnc->encodingArray[index];
}

uint64_t getElementValueUnarySATTranslator(CSolver *This, ElementEncoding *elemEnc) {
	uint i;
	for (i = 0; i < elemEnc->numVars; i++) {
		if (!getValueSolver(This->getSATEncoder()->getCNF()->solver, getEdgeVar( elemEnc->variables[i] )) ) {
			break;
		}
	}

	return elemEnc->encodingArray[i];
}

uint64_t getElementValueSATTranslator(CSolver *This, Element *element) {
	ElementEncoding *elemEnc = element->getElementEncoding();
	if (elemEnc->numVars == 0)//case when the set has only one item
		return element->getRange()->getElement(0);
	switch (elemEnc->type) {
	case ONEHOT:
		return getElementValueOneHotSATTranslator(This, elemEnc);
	case UNARY:
		return getElementValueUnarySATTranslator(This, elemEnc);
	case BINARYINDEX:
		return getElementValueBinaryIndexSATTranslator(This, elemEnc);
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

bool getBooleanVariableValueSATTranslator( CSolver *This, Boolean *boolean) {
	if (boolean->boolVal == BV_MUSTBETRUE)
		return true;
	else if (boolean->boolVal == BV_MUSTBEFALSE)
		return false;
	else {
		int index = getEdgeVar( ((BooleanVar *) boolean)->var );
		return getValueSolver(This->getSATEncoder()->getCNF()->solver, index);
	}
}


