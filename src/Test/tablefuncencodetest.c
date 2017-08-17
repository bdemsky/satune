#include "csolver.h"
/**
 * e1 = {1, 2}
 * e2={3, 5, 7}
 * e3= f(e1, e2)
 *	1 5 => 7
 *	2 3 => 5
 *	1 7 => 3
 *	2 7 => 5
 *	2 5 => 3
 *	1 3 => 5
 * e4 = {6, 10, 19}
 * e4 <= e3
 * Result: e1=1, e2=5, e3=7, e4=6, overflow=0
 */
int main(int numargs, char **argv) {
	CSolver *solver = allocCSolver();
	uint64_t set1[] = {1, 2};
	uint64_t set2[] = {3, 5, 7};
	uint64_t set3[] = {6, 10, 19};
	Set *s1 = createSet(solver, 0, set1, 2);
	Set *s2 = createSet(solver, 0, set2, 3);
	Set *s3 = createSet(solver, 0, set3, 3);
	Element *e1 = getElementVar(solver, s1);
	Element *e2 = getElementVar(solver, s2);
	Element *e4 = getElementVar(solver, s3);
	Boolean *overflow = getBooleanVar(solver, 2);
	Set *d1[] = {s1, s2};
	//change the overflow flag
	Table *t1 = createTable(solver, d1, 2, s2);
	uint64_t row1[] = {1, 5};
	uint64_t row2[] = {2, 3};
	uint64_t row3[] = {1, 7};
	uint64_t row4[] = {2, 7};
	uint64_t row5[] = {2, 5};
	uint64_t row6[] = {1, 3};
	addTableEntry(solver, t1, row1, 2, 7);
	addTableEntry(solver, t1, row2, 2, 5);
	addTableEntry(solver, t1, row3, 2, 3);
	addTableEntry(solver, t1, row4, 2, 5);
	addTableEntry(solver, t1, row5, 2, 3);
	addTableEntry(solver, t1, row6, 2, 5);
	Function *f1 = completeTable(solver, t1, FLAGIFFUNDEFINED);
	Element *e3 = applyFunction(solver, f1, (Element * []) {e1,e2}, 2, overflow);

	Set *deq[] = {s3,s2};
	Predicate *lte = createPredicateOperator(solver, LTE, deq, 2);
	Element *inputs2 [] = {e4, e3};
	Boolean *pred = applyPredicate(solver, lte, inputs2, 2);
	addConstraint(solver, pred);

	if (startEncoding(solver) == 1)
		printf("e1=%llu e2=%llu e3=%llu e4=%llu overFlow:%d\n",
					 getElementValue(solver,e1), getElementValue(solver, e2), getElementValue(solver, e3),
					 getElementValue(solver, e4), getBooleanValue(solver, overflow));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}
