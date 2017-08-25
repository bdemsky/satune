#include "naiveencoder.h"
#include "elementencoding.h"
#include "element.h"
#include "functionencoding.h"
#include "function.h"
#include "set.h"
#include "common.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "table.h"
#include "tableentry.h"
#include "order.h"
#include <strings.h>

void naiveEncodingDecision(CSolver *This) {
	HSIteratorBoolean *iterator=iteratorBoolean(This->constraints);
	while(hasNextBoolean(iterator)) {
		Boolean *boolean = nextBoolean(iterator);
		naiveEncodingConstraint(boolean);
	}
	deleteIterBoolean(iterator);
}

void naiveEncodingConstraint(Boolean *This) {
	switch (GETBOOLEANTYPE(This)) {
	case BOOLEANVAR: {
		return;
	}
	case ORDERCONST: {
		((BooleanOrder *) This)->order->setOrderEncodingType(PAIRWISE);
		return;
	}
	case LOGICOP: {
		naiveEncodingLogicOp((BooleanLogic *) This);
		return;
	}
	case PREDICATEOP: {
		naiveEncodingPredicate((BooleanPredicate *) This);
		return;
	}
	default:
		ASSERT(0);
	}
}

void naiveEncodingLogicOp(BooleanLogic *This) {
	for (uint i = 0; i < This->inputs.getSize(); i++) {
		naiveEncodingConstraint(This->inputs.get(i));
	}
}

void naiveEncodingPredicate(BooleanPredicate *This) {
	FunctionEncoding *encoding = This->getFunctionEncoding();
	if (getFunctionEncodingType(encoding) == FUNC_UNASSIGNED)
		setFunctionEncodingType(This->getFunctionEncoding(), ENUMERATEIMPLICATIONS);

	for (uint i = 0; i < This->inputs.getSize(); i++) {
		Element *element = This->inputs.get(i);
		naiveEncodingElement(element);
	}
}

void naiveEncodingElement(Element *This) {
	ElementEncoding *encoding = getElementEncoding(This);
	if (getElementEncodingType(encoding) == ELEM_UNASSIGNED) {
		setElementEncodingType(encoding, BINARYINDEX);
		encodingArrayInitialization(encoding);
	}

	if (GETELEMENTTYPE(This) == ELEMFUNCRETURN) {
		ElementFunction *function = (ElementFunction *) This;
		for (uint i = 0; i < function->inputs.getSize(); i++) {
			Element *element = function->inputs.get(i);
			naiveEncodingElement(element);
		}
		FunctionEncoding *encoding = getElementFunctionEncoding(function);
		if (getFunctionEncodingType(encoding) == FUNC_UNASSIGNED)
			setFunctionEncodingType(getElementFunctionEncoding(function), ENUMERATEIMPLICATIONS);
	}
}

uint getSizeEncodingArray(ElementEncoding *This, uint setSize) {
	switch (This->type) {
	case BINARYINDEX:
		return NEXTPOW2(setSize);
	case ONEHOT:
	case UNARY:
		return setSize;
	default:
		ASSERT(0);
	}
	return -1;
}

void encodingArrayInitialization(ElementEncoding *This) {
	Element *element = This->element;
	Set *set = getElementSet(element);
	ASSERT(set->isRange == false);
	uint size = set->members->getSize();
	uint encSize = getSizeEncodingArray(This, size);
	allocEncodingArrayElement(This, encSize);
	allocInUseArrayElement(This, encSize);
	for (uint i = 0; i < size; i++) {
		This->encodingArray[i] = set->members->get(i);
		setInUseElement(This, i);
	}
}
