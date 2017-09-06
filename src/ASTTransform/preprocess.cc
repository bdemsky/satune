#include "preprocess.h"
#include "boolean.h"
#include "csolver.h"
#include "tunable.h"

Preprocess::Preprocess(CSolver *_solver)
        : Transform(_solver)
{
}

Preprocess::~Preprocess() {
}

void Preprocess::doTransform() {
	if (solver->getTuner()->getTunable(PREPROCESS, &onoff) == 0)
		return;
	
	SetIteratorBooleanEdge *iterator = solver->getConstraints();
	while (iterator->hasNext()) {
		BooleanEdge boolean = iterator->next();
		Boolean *b = boolean.getBoolean();
		transformBoolean(b);
	}
	delete iterator;
	resolveBooleanVars();
}

void Preprocess::resolveBooleanVars() {
	SetIteratorBoolean * iterator = toremove.iterator();
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

void Preprocess::transformBoolean(Boolean *b) {
	if (!processed.add(b))
		return;
	switch (b->type) {
	case BOOLEANVAR:
		processBooleanVar((BooleanVar *)b);
		break;
	case LOGICOP:
		processLogicOp((BooleanLogic *)b);
		break;
	default:
		break;
	}
}

void Preprocess::processBooleanVar(BooleanVar * b) {
	if (b->polarity==P_TRUE ||
			b->polarity==P_FALSE) {
		toremove.add(b);
	}
}

void Preprocess::processLogicOp(BooleanLogic * b) {
	for(uint i=0; i < b->inputs.getSize(); i++)
		transformBoolean(b->inputs.get(i).getBoolean());
}
