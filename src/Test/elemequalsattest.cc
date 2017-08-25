#include "csolver.h"
/**
 * e1 = {0, 1, 2}
 * e2 = {3, 1, 7}
 * e1 == e2
 *
 * Result (Enumeration):
 *	e1=1 e2=1
 * Result (circuit):
 *	e1=0 e2=3
 *	e1=1 e2=1
 *	e1=2 e2=7
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {0, 1, 2};
	uint64_t set2[] = {3, 1, 7};
	Set *s1 = solver->createSet(0, set1, 3);
	Set *s2 = solver->createSet(0, set2, 3);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	Set *domain[] = {s1, s2};
	Predicate *equals = solver->createPredicateOperator(EQUALS, domain, 2);
	Element *inputs[] = {e1, e2};
	Boolean *b = solver->applyPredicate(equals, inputs, 2);
	solver->addConstraint(b);

	if (solver->startEncoding() == 1)
		printf("e1=%llu e2=%llu\n", solver->getElementValue(e1), solver->getElementValue(e2));
	else
		printf("UNSAT\n");
	delete solver;
}
