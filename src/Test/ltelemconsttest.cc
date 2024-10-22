#include "csolver.h"
/**
 * e1 = 5
 * e2 = {1, 3, 4, 6}
 * e1 < e2
 * Result: e1=5 e2=6
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set3[] = {1, 3, 4, 6};
	Set *s3 = solver->createSet(0, set3, 4);
	Element *e1 = solver->getElementConst(4, 5);
	Element *e2 = solver->getElementVar(s3);
	Predicate *lt = solver->createPredicateOperator(SATC_LT);
	Element *inputs2[] = {e1, e2};
	BooleanEdge b = solver->applyPredicate(lt, inputs2, 2);
	solver->addConstraint(b);
	solver->serialize();
	if (solver->solve() == 1)
		printf("e1=%" PRIu64 " e2=%" PRIu64 "\n", solver->getElementValue(e1), solver->getElementValue(e2));
	else
		printf("UNSAT\n");
	delete solver;
}
