#include "csolver.h"
/**
 * e1 = {6}
 * e2={4, 2}
 * e3=Fsub(e1,e2) {4, 2}
 * e4= f(e1, e2)
 *	6 2 => 3
 *	6 4 => 1
 * e5 = f(e1)=>e1 {6}
 * e6 = (e3, e4, e5) {2, 3, 1}
 *	4 3 6 => 3
 *	2 1 6 => 1
 *	2 3 6 => 2
 *	4 1 6 => 1
 * e7 = {2, 1, 0}
 * e7 > e6
 * Result: e1=6, e2=4, e7=2
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {6};
	uint64_t set2[] = {4, 2};
	uint64_t set3[] = {3, 1};
	uint64_t set4[] = {2, 3, 1};
	uint64_t set5[] = {2, 1, 0};
	Set *s1 = solver->createSet(0, set1, 1);
	Set *s2 = solver->createSet(0, set2, 2);
	Set *s3 = solver->createSet(0, set3, 2);
	Set *s4 = solver->createSet(0, set4, 3);
	Set *s5 = solver->createSet(0, set5, 3);
	Element *e1 = solver->getElementVar(s1);
	Element *e2 = solver->getElementVar(s2);
	Element *e7 = solver->getElementVar(s5);
	Boolean *overflow = solver->getBooleanVar(2);
	Set *d1[] = {s1, s2};
	//change the overflow flag
	Function *f1 = solver->createFunctionOperator(SUB, d1, 2, s2, IGNORE);
	Element *in1[] = {e1, e2};
	Element *e3 = solver->applyFunction(f1, in1, 2, overflow);
	Table *t1 = solver->createTable(d1, 2, s3);
	uint64_t row1[] = {6, 2};
	uint64_t row2[] = {6, 4};
	solver->addTableEntry(t1, row1, 2, 3);
	solver->addTableEntry(t1, row2, 2, 1);
	Function *f2 = solver->completeTable(t1, IGNOREBEHAVIOR);
	Element *e4 = solver->applyFunction(f2, in1, 2, overflow);

	Set *d2[] = {s1};
	Element *in2[] = {e1};
	Table *t2 = solver->createTable(d2, 1, s1);
	uint64_t row3[] = {6};
	solver->addTableEntry(t2, row3, 1, 6);
	Function *f3 = solver->completeTable(t2, IGNOREBEHAVIOR);
	Element *e5 = solver->applyFunction(f3, in2, 1, overflow);

	Set *d3[] = {s2, s3, s1};
	Element *in3[] = {e3, e4, e5};
	Table *t3 = solver->createTable(d3, 3, s4);
	uint64_t row4[] = {4, 3, 6};
	uint64_t row5[] = {2, 1, 6};
	uint64_t row6[] = {2, 3, 6};
	uint64_t row7[] = {4, 1, 6};
	solver->addTableEntry(t3, row4, 3, 3);
	solver->addTableEntry(t3, row5, 3, 1);
	solver->addTableEntry(t3, row6, 3, 2);
	solver->addTableEntry(t3, row7, 3, 1);
	Function *f4 = solver->completeTable(t3, IGNOREBEHAVIOR);
	Element *e6 = solver->applyFunction(f4, in3, 3, overflow);

	Set *deq[] = {s5,s4};
	Predicate *gt = solver->createPredicateOperator(GT, deq, 2);
	Element *inputs2 [] = {e7, e6};
	Boolean *pred = solver->applyPredicate(gt, inputs2, 2);
	solver->addConstraint(pred);

	if (solver->startEncoding() == 1)
		printf("e1=%llu e2=%llu e7=%llu\n",
					 solver->getElementValue(e1), solver->getElementValue(e2), solver->getElementValue(e7));
	else
		printf("UNSAT\n");
	delete solver;
}
