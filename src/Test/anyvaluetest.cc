#include "csolver.h"


int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {10, 8, 18, 20, 21, 22, 23, 24, 25, 26};
	uint64_t set2[] = {10, 13, 7, 30, 31, 32};
	Set *s1 = solver->createSet(0, set1, 10);
	Set *s2 = solver->createSet(1, set2, 6);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	solver->mustHaveValue(e1);
	solver->mustHaveValue(e2);

	Predicate *equals = solver->createPredicateOperator(SATC_EQUALS);
	Element *inputs[] = {e1, e2};
	BooleanEdge b = solver->applyPredicate(equals, inputs, 2);
	b = solver->applyLogicalOperation(SATC_NOT, b);
	solver->addConstraint(b);

	if (solver->solve() == 1)
		printf("e1=%" PRIu64 "e2=%" PRIu64 "\n", solver->getElementValue(e1), solver->getElementValue(e2));
	else
		printf("UNSAT\n");
	delete solver;
}
