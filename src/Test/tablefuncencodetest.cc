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
	CSolver *solver = new CSolver();
	uint64_t set1[] = {1, 2};
	uint64_t set2[] = {3, 5, 7};
	uint64_t set3[] = {6, 10, 19};
	Set *s1 = solver->createSet(0, set1, 2);
	Set *s2 = solver->createSet(0, set2, 3);
	Set *s3 = solver->createSet(0, set3, 3);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	Element *e4 = solver->getElementVar(s3);
	Boolean *overflow = solver->getBooleanVar(2);
	Set *d1[] = {s1, s2};
	//change the overflow flag
	Table *t1 = solver->createTable(d1, 2, s2);
	uint64_t row1[] = {1, 5};
	uint64_t row2[] = {2, 3};
	uint64_t row3[] = {1, 7};
	uint64_t row4[] = {2, 7};
	uint64_t row5[] = {2, 5};
	uint64_t row6[] = {1, 3};
	solver->addTableEntry(t1, row1, 2, 7);
	solver->addTableEntry(t1, row2, 2, 5);
	solver->addTableEntry(t1, row3, 2, 3);
	solver->addTableEntry(t1, row4, 2, 5);
	solver->addTableEntry(t1, row5, 2, 3);
	solver->addTableEntry(t1, row6, 2, 5);
	Function *f1 = solver->completeTable(t1, FLAGIFFUNDEFINED);
	Element *tmparray[] = {e1, e2};
	Element *e3 = solver->applyFunction(f1, tmparray, 2, overflow);

	Set *deq[] = {s3,s2};
	Predicate *lte = solver->createPredicateOperator(LTE, deq, 2);
	Element *inputs2 [] = {e4, e3};
	Boolean *pred = solver->applyPredicate(lte, inputs2, 2);
	solver->addConstraint(pred);

	if (solver->startEncoding() == 1)
		printf("e1=%" PRIu64 " e2=%" PRIu64 " e3=%" PRIu64 " e4=%" PRIu64 " overFlow:%d\n",
					 solver->getElementValue(e1), solver->getElementValue(e2), solver->getElementValue(e3),
					 solver->getElementValue(e4), solver->getBooleanValue(overflow));
	else
		printf("UNSAT\n");
	delete solver;
}
