#include "polarityassignment.h"
#include "csolver.h"

void computePolarities(CSolver *This) {
	SetIteratorBooleanEdge *iterator = This->getConstraints();
	while (iterator->hasNext()) {
		BooleanEdge boolean = iterator->next();
		Boolean *b = boolean.getBoolean();
		bool isNegated = boolean.isNegated();
		if (isNegated) {
			updatePolarity(b, P_FALSE);
		} else {
			updatePolarity(b, P_TRUE);
		}
		computePolarity(b);
	}
	delete iterator;
}

void updatePolarity(Boolean *This, Polarity polarity) {
	This->polarity = (Polarity) (This->polarity | polarity);
}

void updateMustValue(Boolean *This, BooleanValue value) {
	This->boolVal = (BooleanValue) (This->boolVal | value);
}

void computePolarity(Boolean *This) {
	switch (This->type) {
	case BOOLEANVAR:
	case ORDERCONST:
		return;
	case PREDICATEOP:
		return computePredicatePolarity((BooleanPredicate *)This);
	case LOGICOP:
		return computeLogicOpPolarity((BooleanLogic *)This);
	default:
		ASSERT(0);
	}
}

void computePredicatePolarity(BooleanPredicate *This) {
	if (This->undefStatus) {
		updatePolarity(This->undefStatus.getBoolean(), P_BOTHTRUEFALSE);
		computePolarity(This->undefStatus.getBoolean());
	}
}

void computeLogicOpPolarity(BooleanLogic *This) {
	computeLogicOpPolarityChildren(This);
	uint size = This->inputs.getSize();
	for (uint i = 0; i < size; i++) {
		computePolarity(This->inputs.get(i).getBoolean());
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

void computeLogicOpPolarityChildren(BooleanLogic *This) {
	Polarity parentpolarity = This->polarity;
	switch (This->op) {
	case SATC_AND: {
		uint size = This->inputs.getSize();
		for (uint i = 0; i < size; i++) {
			BooleanEdge tmp = This->inputs.get(i);
			Boolean *btmp = tmp.getBoolean();
			updatePolarity(btmp, tmp.isNegated() ? negatePolarity(parentpolarity) : parentpolarity);
		}
		break;
	}
	case SATC_IFF: {
		updatePolarity(This->inputs.get(0).getBoolean(), P_BOTHTRUEFALSE);
		updatePolarity(This->inputs.get(1).getBoolean(), P_BOTHTRUEFALSE);
		break;
	}
	default:
		ASSERT(0);
	}
}
