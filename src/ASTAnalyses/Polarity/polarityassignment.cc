#include "polarityassignment.h"
#include "csolver.h"

void computePolarities(CSolver *This) {
	if(This->isUnSAT()){
		return;
	}
	SetIteratorBooleanEdge *iterator = This->getConstraints();
	while (iterator->hasNext()) {
		BooleanEdge boolean = iterator->next();
		Boolean *b = boolean.getBoolean();
		bool isNegated = boolean.isNegated();
		computePolarity(b, isNegated ? P_FALSE : P_TRUE);
	}
	delete iterator;
}

void updateEdgePolarity(BooleanEdge dst, BooleanEdge src) {
	Boolean *bdst = dst.getBoolean();
	Boolean *bsrc = src.getBoolean();
	bool isNegated = dst.isNegated() ^ src.isNegated();
	Polarity p = isNegated ? negatePolarity(bsrc->polarity) : bsrc->polarity;
	updatePolarity(bdst, p);
}

void updateEdgePolarity(BooleanEdge dst, Polarity p) {
	Boolean *bdst = dst.getBoolean();
	bool isNegated = dst.isNegated();
	if (isNegated)
		p = negatePolarity(p);
	updatePolarity(bdst, p);
}

bool updatePolarity(Boolean *This, Polarity polarity) {
	Polarity oldpolarity = This->polarity;
	Polarity newpolarity = (Polarity) (This->polarity | polarity);
	This->polarity = newpolarity;
	return newpolarity != oldpolarity;
}

void updateMustValue(Boolean *This, BooleanValue value) {
	This->boolVal = (BooleanValue) (This->boolVal | value);
}

void computePolarity(Boolean *This, Polarity polarity) {
	if (updatePolarity(This, polarity)) {
		switch (This->type) {
		case BOOLEANVAR:
		case ORDERCONST:
		case BOOLCONST:
			return;
		case PREDICATEOP:
			return computePredicatePolarity((BooleanPredicate *)This);
		case LOGICOP:
			return computeLogicOpPolarity((BooleanLogic *)This);
		default:
			ASSERT(0);
		}
	}
}

void computePredicatePolarity(BooleanPredicate *This) {
	if (This->undefStatus) {
		computePolarity(This->undefStatus.getBoolean(), P_BOTHTRUEFALSE);
	}
	for (uint i = 0; i < This->inputs.getSize(); i++) {
		Element *e = This->inputs.get(i);
		computeElement(e);
	}
}

void computeElement(Element *e) {
	if (e->type == ELEMFUNCRETURN) {
		ElementFunction *ef = (ElementFunction *) e;

		if (ef->overflowstatus) {
			computePolarity(ef->overflowstatus.getBoolean(), P_BOTHTRUEFALSE);
		}

		for (uint i = 0; i < ef->inputs.getSize(); i++) {
			Element *echild = ef->inputs.get(i);
			computeElement(echild);
		}
	}
}

void computeLogicOpPolarity(BooleanLogic *This) {
	Polarity child = computeLogicOpPolarityChildren(This);
	uint size = This->inputs.getSize();
	for (uint i = 0; i < size; i++) {
		BooleanEdge b = This->inputs.get(i);
		computePolarity(b.getBoolean(), b.isNegated() ? negatePolarity(child) : child);
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

Polarity computeLogicOpPolarityChildren(BooleanLogic *This) {
	switch (This->op) {
	case SATC_AND: {
		return This->polarity;
	}
	case SATC_IFF: {
		return P_BOTHTRUEFALSE;
	}
	default:
		ASSERT(0);
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
