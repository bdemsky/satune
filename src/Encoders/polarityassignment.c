#include "polarityassignment.h"
#include "csolver.h"

void computePolarities(CSolver *This) {
	HSIteratorBoolean *iterator=iteratorBoolean(This->constraints);
	while(hasNextBoolean(iterator)) {
		Boolean *boolean = nextBoolean(iterator);
		updatePolarity(boolean, P_TRUE);
		updateMustValue(boolean, BV_MUSTBETRUE);
		computePolarityAndBooleanValue(boolean);
	}
	deleteIterBoolean(iterator);
}

void updatePolarity(Boolean *This, Polarity polarity) {
	This->polarity |= polarity;
}

void updateMustValue(Boolean *This, BooleanValue value) {
	This->boolVal |= value;
}

void computePolarityAndBooleanValue(Boolean *This) {
	switch (GETBOOLEANTYPE(This)) {
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
	updatePolarity(This->undefStatus, P_BOTHTRUEFALSE);
	computePolarityAndBooleanValue(This->undefStatus);
}

void computeLogicOpPolarityAndBooleanValue(BooleanLogic *This) {
	computeLogicOpBooleanValue(This);
	computeLogicOpPolarity(This);
	uint size = getSizeArrayBoolean(&This->inputs);
	for (uint i = 0; i < size; i++) {
		computePolarityAndBooleanValue(getArrayBoolean(&This->inputs, i));
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
	Polarity parentpolarity = GETBOOLEANPOLARITY(This);
	switch (This->op) {
	case L_AND:
	case L_OR: {
		uint size = getSizeArrayBoolean(&This->inputs);
		for (uint i = 0; i < size; i++) {
			Boolean *tmp = getArrayBoolean(&This->inputs, i);
			updatePolarity(tmp, parentpolarity);
		}
		break;
	}
	case L_NOT: {
		Boolean *tmp = getArrayBoolean(&This->inputs, 0);
		updatePolarity(tmp, negatePolarity(parentpolarity));
		break;
	}
	case L_XOR: {
		updatePolarity(getArrayBoolean(&This->inputs, 0), P_BOTHTRUEFALSE);
		updatePolarity(getArrayBoolean(&This->inputs, 1), P_BOTHTRUEFALSE);
		break;
	}
	case L_IMPLIES: {
		Boolean *left = getArrayBoolean(&This->inputs, 0);
		updatePolarity(left, negatePolarity( parentpolarity));
		Boolean *right = getArrayBoolean(&This->inputs, 1);
		updatePolarity(right, parentpolarity);
		break;
	}
	default:
		ASSERT(0);
	}
}

void computeLogicOpBooleanValue(BooleanLogic *This) {
	BooleanValue parentbv = GETBOOLEANVALUE(This);
	switch (This->op) {
	case L_AND: {
		if (parentbv == BV_MUSTBETRUE || parentbv == BV_UNSAT) {
			uint size = getSizeArrayBoolean(&This->inputs);
			for (uint i = 0; i < size; i++) {
				updateMustValue(getArrayBoolean(&This->inputs, i), parentbv);
			}
		}
		return;
	}
	case L_OR: {
		if (parentbv == BV_MUSTBEFALSE || parentbv == BV_UNSAT) {
			uint size = getSizeArrayBoolean(&This->inputs);
			for (uint i = 0; i < size; i++) {
				updateMustValue(getArrayBoolean(&This->inputs, i), parentbv);
			}
		}
		return;
	}
	case L_NOT:
		updateMustValue(getArrayBoolean(&This->inputs, 0), negateBooleanValue(parentbv));
		return;
	case L_IMPLIES:
		//implies is really an or with the first term negated
		if (parentbv == BV_MUSTBEFALSE || parentbv == BV_UNSAT) {
			uint size = getSizeArrayBoolean(&This->inputs);
			updateMustValue(getArrayBoolean(&This->inputs, 0), negateBooleanValue(parentbv));
			updateMustValue(getArrayBoolean(&This->inputs, 1), parentbv);
		}
		return;
	case L_XOR:
		return;
	default:
		ASSERT(0);
	}
}
