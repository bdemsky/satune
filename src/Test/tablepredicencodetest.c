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
int main(int numargs, char ** argv) {
	CSolver * solver=allocCSolver();
	uint64_t set1[]={1, 2};
	uint64_t set2[]={1, 3, 5, 7};
	uint64_t set3[]={6, 10, 19};
	Set * s1=createSet(solver, 0, set1, 2);
	Set * s2=createSet(solver, 0, set2, 4);
	Set * s3=createSet(solver, 0, set3, 3);
	Element * e1=getElementVar(solver, s1);
	Element * e2=getElementVar(solver, s2);
	Element * e3=getElementVar(solver, s3);
	Set * d2[]={s1, s2, s3};
	//change the overflow flag
	Table* t1 = createTableForPredicate(solver, d2, 3);
	uint64_t row1[] = {1, 5, 6};
	uint64_t row2[] = {2, 3, 19};
	uint64_t row3[] = {1, 3, 19};
	uint64_t row4[] = {2, 7, 10};
	uint64_t row5[] = {1, 7, 6};
	uint64_t row6[] = {2, 5, 6};
	addTableEntry(solver, t1, row1, 3, true);
	addTableEntry(solver, t1, row2, 3, true);
	addTableEntry(solver, t1, row3, 3, false);
	addTableEntry(solver, t1, row4, 3, false);
	addTableEntry(solver, t1, row5, 3, false);
	addTableEntry(solver, t1, row6, 3, true);
	Predicate * p1 = createPredicateTable(solver, t1, FLAGIFFUNDEFINED);
	Boolean* undef = getBooleanVar(solver , 2);
	Boolean* b1 =applyPredicateTable(solver, p1, (Element* []){e1, e2, e3}, 3, undef);
	addConstraint(solver, b1);
	
	Set* deq[] = {s3,s2};
	Predicate* gte = createPredicateOperator(solver, GTE, deq, 2);
	Element* inputs2 [] = {e3, e2};
	Boolean* pred = applyPredicate(solver, gte, inputs2, 2);
	addConstraint(solver, pred);
	
	Set * d1[]={s1, s2};
	Predicate* eq = createPredicateOperator(solver, EQUALS, d1, 2);
	Boolean* pred2 = applyPredicate(solver, eq,(Element*[]) {e1, e2}, 2);
	addConstraint(solver, pred2);
	
	if (startEncoding(solver)==1)
		printf("e1=%llu e2=%llu e3=%llu undefFlag:%d\n", 
			getElementValue(solver,e1), getElementValue(solver, e2), 
			getElementValue(solver, e3), getBooleanValue(solver, undef));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}
