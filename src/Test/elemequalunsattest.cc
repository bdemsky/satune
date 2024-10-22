#include "csolver.h"
/**
 * e1 = {0, 1, 2}
 * e2 = {3, 4}
 * e1 == e2
 *
 * Result: UNSAT
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {0, 1, 2};
	uint64_t set2[] = {3, 4};
	Set *s1 = solver->createSet(0, set1, 3);
	Set *s2 = solver->createSet(0, set2, 2);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	Predicate *equals = solver->createPredicateOperator(SATC_EQUALS);
	Element *inputs[] = {e1, e2};
	BooleanEdge b = solver->applyPredicate(equals, inputs, 2);
	solver->addConstraint(b);
	solver->serialize();
	if (solver->solve() == 1)
		printf("e1=%" PRIu64 " e2=%" PRIu64 "\n", solver->getElementValue(e1), solver->getElementValue(e2));
	else
		printf("UNSAT\n");
	delete solver;
}
