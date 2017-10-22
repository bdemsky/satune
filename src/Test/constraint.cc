#include "csolver.h"

int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t elements[] = {0, 1};
	Set *s = solver->createSet(1, elements, 2);
	Element *e1 = solver->getElementVar(s);
	Element *e2 = solver->getElementVar(s);
	Set *sarray[] = {s, s};
	Predicate *p = solver->createPredicateOperator(SATC_LT, sarray, 2);
	Element *earray[] = {e1, e2};
	BooleanEdge be = solver->applyPredicate(p, earray, 2);
	solver->addConstraint(be);

	if (solver->solve() == 1) {
		printf("SAT\n");
	} else {
		printf("UNSAT\n");
	}
	delete solver;
}
