#include "csolver.h"
/**
 * e1 = {1, 2}
 * e2={1, 3, 5, 7}
 * e3 = {6, 10, 19}
 * e4= p(e1, e2, e3)
 *	1 5 6 => T
 *	2 3 19 => T
 *	1 3 19 => F
 *	2 7 10 => F
 *	1 7 6 => F
 *	2 5 6 => T
 * e1 == e2
 * e3 >= e2
 * Result: e1=1, e2=1, e3=6 OR 10 OR 19, overflow=1
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {1, 2};
	uint64_t set2[] = {1, 3, 5, 7};
	uint64_t set3[] = {6, 10, 19};
	Set *s1 = solver->createSet(0, set1, 2);
	Set *s2 = solver->createSet(0, set2, 4);
	Set *s3 = solver->createSet(0, set3, 3);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	Element *e3 = solver->getElementVar(s3);
	Set *d2[] = {s1, s2, s3};
	//change the overflow flag
	Table *t1 = solver->createTableForPredicate(d2, 3);
	uint64_t row1[] = {1, 5, 6};
	uint64_t row2[] = {2, 3, 19};
	uint64_t row3[] = {1, 3, 19};
	uint64_t row4[] = {2, 7, 10};
	uint64_t row5[] = {1, 7, 6};
	uint64_t row6[] = {2, 5, 6};
	solver->addTableEntry(t1, row1, 3, true);
	solver->addTableEntry(t1, row2, 3, true);
	solver->addTableEntry(t1, row3, 3, false);
	solver->addTableEntry(t1, row4, 3, false);
	solver->addTableEntry(t1, row5, 3, false);
	solver->addTableEntry(t1, row6, 3, true);
	Predicate *p1 = solver->createPredicateTable(t1, SATC_FLAGIFFUNDEFINED);
	Boolean *undef = solver->getBooleanVar(2);
	Element *tmparray[] = {e1, e2, e3};
	Boolean *b1 = solver->applyPredicateTable(p1, tmparray, 3, undef);
	solver->addConstraint(b1);

	Set *deq[] = {s3,s2};
	Predicate *gte = solver->createPredicateOperator(SATC_GTE, deq, 2);
	Element *inputs2 [] = {e3, e2};
	Boolean *pred = solver->applyPredicate(gte, inputs2, 2);
	solver->addConstraint(pred);

	Set *d1[] = {s1, s2};
	Predicate *eq = solver->createPredicateOperator(SATC_EQUALS, d1, 2);
	Element *tmparray2[] = {e1, e2};
	Boolean *pred2 = solver->applyPredicate(eq, tmparray2, 2);
	solver->addConstraint(pred2);

	if (solver->startEncoding() == 1)
		printf("e1=%" PRIu64 " e2=%" PRIu64 " e3=%" PRIu64 " undefFlag:%d\n",
					 solver->getElementValue(e1), solver->getElementValue(e2),
					 solver->getElementValue(e3), solver->getBooleanValue(undef));
	else
		printf("UNSAT\n");
	delete solver;
}
