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
	HSIteratorBoolean *iterator = This->getConstraints();
	while (iterator->hasNext()) {
		Boolean *boolean = iterator->next();
		naiveEncodingConstraint(boolean);
	}
	delete iterator;
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
	if (encoding->getFunctionEncodingType() == FUNC_UNASSIGNED)
		This->getFunctionEncoding()->setFunctionEncodingType(ENUMERATEIMPLICATIONS);

	for (uint i = 0; i < This->inputs.getSize(); i++) {
		Element *element = This->inputs.get(i);
		naiveEncodingElement(element);
	}
}

void naiveEncodingElement(Element *This) {
	ElementEncoding *encoding = getElementEncoding(This);
	if (encoding->getElementEncodingType() == ELEM_UNASSIGNED) {
		encoding->setElementEncodingType(BINARYINDEX);
		encoding->encodingArrayInitialization();
	}

	if (GETELEMENTTYPE(This) == ELEMFUNCRETURN) {
		ElementFunction *function = (ElementFunction *) This;
		for (uint i = 0; i < function->inputs.getSize(); i++) {
			Element *element = function->inputs.get(i);
			naiveEncodingElement(element);
		}
		FunctionEncoding *encoding = getElementFunctionEncoding(function);
		if (encoding->getFunctionEncodingType() == FUNC_UNASSIGNED)
			getElementFunctionEncoding(function)->setFunctionEncodingType(ENUMERATEIMPLICATIONS);
	}
}

