#include "polarityassignment.h"
#include "csolver.h"

void computePolarities(CSolver *This) {
	SetIteratorBoolean *iterator = This->getConstraints();
	while (iterator->hasNext()) {
		Boolean *boolean = iterator->next();
		updatePolarity(boolean, P_TRUE);
		updateMustValue(boolean, BV_MUSTBETRUE);
		computePolarityAndBooleanValue(boolean);
	}
	delete iterator;
}

void updatePolarity(Boolean *This, Polarity polarity) {
	This->polarity = (Polarity) (This->polarity | polarity);
}

void updateMustValue(Boolean *This, BooleanValue value) {
	This->boolVal = (BooleanValue) (This->boolVal | value);
}

void computePolarityAndBooleanValue(Boolean *This) {
	switch (This->type) {
	case BOOLEANVAR:
	case ORDERCONST:
		return;
	case PREDICATEOP:
		return computePredicatePolarityAndBooleanValue((BooleanPredicate *)This);
	case LOGICOP:
		return computeLogicOpPolarityAndBooleanValue((BooleanLogic *)This);
	default:
		ASSERT(0);
	}
}

void computePredicatePolarityAndBooleanValue(BooleanPredicate *This) {
	if (This->undefStatus != NULL) {
		updatePolarity(This->undefStatus, P_BOTHTRUEFALSE);
		computePolarityAndBooleanValue(This->undefStatus);
	}
}

void computeLogicOpPolarityAndBooleanValue(BooleanLogic *This) {
	computeLogicOpBooleanValue(This);
	computeLogicOpPolarity(This);
	uint size = This->inputs.getSize();
	for (uint i = 0; i < size; i++) {
		computePolarityAndBooleanValue(This->inputs.get(i));
	}
}

Polarity negatePolarity(Polarity This) {
	switch (This) {
	case P_UNDEFINED:
	case P_BOTHTRUEFALSE:
		return This;
	case P_TRUE:
		return P_FALSE;
	case P_FALSE:
		return P_TRUE;
	default:
		ASSERT(0);
	}
}

BooleanValue negateBooleanValue(BooleanValue This) {
	switch (This) {
	case BV_UNDEFINED:
	case BV_UNSAT:
		return This;
	case BV_MUSTBETRUE:
		return BV_MUSTBEFALSE;
	case BV_MUSTBEFALSE:
		return BV_MUSTBETRUE;
	default:
		ASSERT(0);
	}
}

void computeLogicOpPolarity(BooleanLogic *This) {
	Polarity parentpolarity = This->polarity;
	switch (This->op) {
	case SATC_AND: {
		uint size = This->inputs.getSize();
		for (uint i = 0; i < size; i++) {
			Boolean *tmp = This->inputs.get(i);
			updatePolarity(tmp, parentpolarity);
		}
		break;
	}
	case SATC_NOT: {
		Boolean *tmp = This->inputs.get(0);
		updatePolarity(tmp, negatePolarity(parentpolarity));
		break;
	}
	case SATC_IFF: {
		updatePolarity(This->inputs.get(0), P_BOTHTRUEFALSE);
		updatePolarity(This->inputs.get(1), P_BOTHTRUEFALSE);
		break;
	}
	default:
		ASSERT(0);
	}
}

void computeLogicOpBooleanValue(BooleanLogic *This) {
	BooleanValue parentbv = This->boolVal;
	switch (This->op) {
	case SATC_AND: {
		if (parentbv == BV_MUSTBETRUE || parentbv == BV_UNSAT) {
			uint size = This->inputs.getSize();
			for (uint i = 0; i < size; i++) {
				updateMustValue(This->inputs.get(i), parentbv);
			}
		}
		return;
	}
	case SATC_NOT:
		updateMustValue(This->inputs.get(0), negateBooleanValue(parentbv));
		return;
	case SATC_IFF:
		return;
	default:
		ASSERT(0);
	}
}
