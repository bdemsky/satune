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
	Predicate *equals = solver->createPredicateOperator(SATC_EQUALS);
	Element *inputs[] = {e1, e2};
	BooleanEdge b = solver->applyPredicate(equals, inputs, 2);
	solver->addConstraint(b);

	Function *f1 = solver->createFunctionOperator(SATC_ADD, setbig, SATC_IGNORE);

	Table *table = solver->createTable(s);
	uint64_t row1[] = {0, 1};
	uint64_t row2[] = {1, 1};
	uint64_t row3[] = {2, 1};
	uint64_t row4[] = {2, 2};
	solver->addTableEntry(table, row1, 2, 0);
	solver->addTableEntry(table, row2, 2, 0);
	solver->addTableEntry(table, row3, 2, 2);
	solver->addTableEntry(table, row4, 2, 2);
	Function *f2 = solver->completeTable(table, SATC_IGNOREBEHAVIOR);	//its range would be as same as s
	BooleanEdge overflow = solver->getBooleanVar(2);
	Element *e3 = solver->applyFunction(f1, inputs, 2, overflow);
	Element *e4 = solver->applyFunction(f2, inputs, 2, overflow);
	Predicate *equal2 = solver->createPredicateOperator(SATC_EQUALS);
	Element *inputs2 [] = {e4, e3};
	BooleanEdge pred = solver->applyPredicate(equal2, inputs2, 2);
	solver->addConstraint(pred);
	solver->serialize();
	if (solver->solve() == 1)
		printf("e1=%" PRIu64 " e2=%" PRIu64 " \n", solver->getElementValue(e1), solver->getElementValue(e2));
	else
		printf("UNSAT\n");
	delete solver;
}
