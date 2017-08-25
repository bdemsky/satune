#include "csolver.h"

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
	uint64_t set1[] = {0, 1, 2};
	uint64_t setbigarray[] = {0, 1, 2, 3, 4};

	Set *s = solver->createSet(0, set1, 3);
	Set *setbig = solver->createSet(0, setbigarray, 5);
	Element *e1 = solver->getElementVar(s);
	Element *e2 = solver->getElementVar(s);
	Set *domain[] = {s, s};
	Predicate *equals = solver->createPredicateOperator(EQUALS, domain, 2);
	Element *inputs[] = {e1, e2};
	Boolean *b = solver->applyPredicate(equals, inputs, 2);
	solver->addConstraint(b);

	uint64_t set2[] = {2, 3};
	Set *rangef1 = solver->createSet(1, set2, 2);
	Function *f1 = solver->createFunctionOperator(ADD, domain, 2, setbig, IGNORE);

	Table *table = solver->createTable(domain, 2, s);
	uint64_t row1[] = {0, 1};
	uint64_t row2[] = {1, 1};
	uint64_t row3[] = {2, 1};
	uint64_t row4[] = {2, 2};
	solver->addTableEntry(table, row1, 2, 0);
	solver->addTableEntry(table, row2, 2, 0);
	solver->addTableEntry(table, row3, 2, 2);
	solver->addTableEntry(table, row4, 2, 2);
	Function *f2 = solver->completeTable(table, IGNOREBEHAVIOR);	//its range would be as same as s
	Boolean *overflow = solver->getBooleanVar(2);
	Element *e3 = solver->applyFunction(f1, inputs, 2, overflow);
	Element *e4 = solver->applyFunction(f2, inputs, 2, overflow);
	Set *domain2[] = {s,rangef1};
	Predicate *equal2 = solver->createPredicateOperator(EQUALS, domain2, 2);
	Element *inputs2 [] = {e4, e3};
	Boolean *pred = solver->applyPredicate(equal2, inputs2, 2);
	solver->addConstraint(pred);

	if (solver->startEncoding() == 1)
		printf("e1=%llu e2=%llu\n", solver->getElementValue(e1), solver->getElementValue(e2));
	else
		printf("UNSAT\n");
	delete solver;
}
