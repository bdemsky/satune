#include "preprocess.h"
#include "boolean.h"
#include "csolver.h"
#include "tunable.h"
#include "iterator.h"

Preprocess::Preprocess(CSolver *_solver)
	: Transform(_solver)
{
}

Preprocess::~Preprocess() {
}

void Preprocess::doTransform() {
	if (solver->isUnSAT() || !solver->isBooleanVarUsed() || solver->getTuner()->getTunable(PREPROCESS, &onoff) == 0)
		return;

	BooleanIterator bit(solver);
	while (bit.hasNext()) {
		Boolean *b = bit.next();
		if (b->type == BOOLEANVAR)
			processBooleanVar((BooleanVar *)b);
	}
	resolveBooleanVars();
}

void Preprocess::resolveBooleanVars() {
	SetIteratorBoolean *iterator = toremove.iterator();
	while (iterator->hasNext()) {
		BooleanVar *bv = (BooleanVar *) iterator->next();
		if (bv->polarity == P_TRUE) {
			solver->replaceBooleanWithTrue(BooleanEdge(bv));
		} else if (bv->polarity == P_FALSE) {
			solver->replaceBooleanWithFalse(BooleanEdge(bv));
		}
	}
	delete iterator;
}

void Preprocess::processBooleanVar(BooleanVar *b) {
	if (b->polarity == P_TRUE ||
			b->polarity == P_FALSE) {
		toremove.add(b);
	}
}
