#include "elementopt.h"
#include "csolver.h"
#include "tunable.h"
#include "iterator.h"

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
	}
}
