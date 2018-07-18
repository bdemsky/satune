#include "csolver.h"
#include <unistd.h>

/**
 * e1={0, 1, 2}
 * e2={0, 1, 2}
 * e1 == e2
 * e3= e1+e2 {0, 1, 2, 3, 4}
 * e4 = f(e1, e2)
 *	0 1 => 0
 *	1 1 => 0
 *	2 1 => 2
 *	2 2 => 2
 * e3 == e4
 * Result: UNSAT!
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {1, 2, 3};
	Set *s1 = solver->createSet(1, set1, 3);
	Set *s2 = solver->createSet(1, set1, 3);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	solver->mustHaveValue(e1);
	solver->mustHaveValue(e2);
	Set *domain[] = {s1, s2};
	Element *inputs[] = {e1, e2};

	uint64_t set2[] = {3};
	Set *rangef1 = solver->createSet(1, set2, 1);
	Function *f1 = solver->createFunctionOperator(SATC_ADD, domain, 2, rangef1, SATC_FLAGIFFOVERFLOW);

	BooleanEdge overflow = solver->getBooleanVar(2);
	Element *e3 = solver->applyFunction(f1, inputs, 2, overflow);
	Element *e4 = solver->getElementConst(5, 3);
	Set *domain2[] = {rangef1,rangef1};
	Predicate *equal2 = solver->createPredicateOperator(SATC_EQUALS, domain2, 2);
	Element *inputs2 [] = {e4, e3};
	BooleanEdge pred = solver->applyPredicate(equal2, inputs2, 2);
	solver->addConstraint(pred);
	solver->addConstraint(solver->applyLogicalOperation(SATC_NOT, overflow));
	if (solver->solve() == 1)
		printf("e1=%" PRIu64 " e2=%" PRIu64 " \n", solver->getElementValue(e1), solver->getElementValue(e2));
	else
		printf("UNSAT\n");
	delete solver;
}
