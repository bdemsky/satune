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
#include "tunable.h"
#include <strings.h>

void naiveEncodingDecision(CSolver *This) {
	if (This->isUnSAT()) {
		return;
	}
	HashsetBoolean *visited = new HashsetBoolean();
	SetIteratorBooleanEdge *iterator = This->getConstraints();
	while (iterator->hasNext()) {
		BooleanEdge b = iterator->next();
		naiveEncodingConstraint(This, visited, b.getBoolean());
	}
	delete iterator;
	delete visited;
}

void naiveEncodingConstraint(CSolver *csolver, HashsetBoolean *visited, Boolean *This) {
	switch (This->type) {
	case BOOLEANVAR: {
		return;
	}
	case ORDERCONST: {
		if (((BooleanOrder *) This)->order->encoding.type == ORDER_UNASSIGNED)
			((BooleanOrder *) This)->order->setOrderEncodingType(PAIRWISE);
		return;
	}
	case LOGICOP: {
		naiveEncodingLogicOp(csolver, visited, (BooleanLogic *) This);
		return;
	}
	case PREDICATEOP: {
		naiveEncodingPredicate(csolver, (BooleanPredicate *) This);
		return;
	}
	default:
		ASSERT(0);
	}
}

void naiveEncodingLogicOp(CSolver *csolver, HashsetBoolean *visited, BooleanLogic *This) {
	if (!visited->add(This))
		return;

	for (uint i = 0; i < This->inputs.getSize(); i++) {
		naiveEncodingConstraint(csolver, visited, This->inputs.get(i).getBoolean());
	}
}

void naiveEncodingPredicate(CSolver *csolver, BooleanPredicate *This) {
	FunctionEncoding *encoding = This->getFunctionEncoding();
	if (encoding->getFunctionEncodingType() == FUNC_UNASSIGNED)
		This->getFunctionEncoding()->setFunctionEncodingType(ENUMERATEIMPLICATIONS);

	for (uint i = 0; i < This->inputs.getSize(); i++) {
		Element *element = This->inputs.get(i);
		naiveEncodingElement(csolver, element);
	}
}

void naiveEncodingElement(CSolver *csolver, Element *This) {
	ElementEncoding *encoding = This->getElementEncoding();
	if (encoding->getElementEncodingType() == ELEM_UNASSIGNED) {
		if (This->type != ELEMCONST) {
			model_print("INFO: naive encoder is making the decision about element %p....\n", This);
		}
		uint enc = csolver->getTuner()->getVarTunable(This->getRange()->getType(), NODEENCODING, &NodeEncodingDesc);
		if (enc == ELEM_UNASSIGNED)
			enc = csolver->getTuner()->getTunable(NAIVEENCODER, &NaiveEncodingDesc);
		encoding->setElementEncodingType((ElementEncodingType)enc);
		encoding->encodingArrayInitialization();
	}

	if (This->type == ELEMFUNCRETURN) {
		ElementFunction *function = (ElementFunction *) This;
		for (uint i = 0; i < function->inputs.getSize(); i++) {
			Element *element = function->inputs.get(i);
			naiveEncodingElement(csolver, element);
		}
		FunctionEncoding *encoding = function->getElementFunctionEncoding();
		if (encoding->getFunctionEncodingType() == FUNC_UNASSIGNED)
			function->getElementFunctionEncoding()->setFunctionEncodingType(ENUMERATEIMPLICATIONS);
	}
}

