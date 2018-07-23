#include "elementopt.h"
#include "csolver.h"
#include "tunable.h"
#include "iterator.h"
#include "boolean.h"
#include "element.h"
#include "predicate.h"

ElementOpt::ElementOpt(CSolver *_solver)
	: Transform(_solver)
{
}

ElementOpt::~ElementOpt() {
}

void ElementOpt::doTransform() {
	if (solver->getTuner()->getTunable(ELEMENTOPT, &onoff) == 0)
		return;

	BooleanIterator bit(solver);
	while (bit.hasNext()) {
		Boolean *b = bit.next();
		if (b->type == PREDICATEOP)
			processPredicate((BooleanPredicate *)b);
	}
}

void ElementOpt::processPredicate(BooleanPredicate * pred) {
	uint numInputs = pred->inputs.getSize();
	if (numInputs != 2)
		return;

	Predicate * p = pred->getPredicate();
	if (p->type == TABLEPRED)
			return;

	PredicateOperator * pop = (PredicateOperator *) p;
	CompOp op = pop->getOp();

	Element * left = pred->inputs.get(0);
	Element * right = pred->inputs.get(1);

	if (left->type == ELEMCONST) {

	} else if (right->type == ELEMCONST) {

	} else {
		return;
	}
}
