#include "csolver.h"
/**
 * e1 = {0, 1, 2}
 * e2 = {3, 1, 7}
 * e1 == e2
 * 
 * Result (Enumeration):
 *	e1=1 e2=1
 * Result (circuit):
 *	e1=0 e2=3 
 *	e1=1 e2=1 
 *	e1=2 e2=7 
 */
int main(int numargs, char ** argv) {
	CSolver * solver=allocCSolver();
	uint64_t set1[]={0, 1, 2};
	uint64_t set2[]={3, 1, 7};
	Set * s1=createSet(solver, 0, set1, 3);
	Set * s2=createSet(solver, 0, set2, 3);
	Element * e1=getElementVar(solver, s1);
	Element * e2=getElementVar(solver, s2);
	Set * domain[]={s1, s2};
	Predicate *equals=createPredicateOperator(solver, EQUALS, domain, 2);
	Element * inputs[]={e1, e2};
	Boolean *b=applyPredicate(solver, equals, inputs, 2);
	addConstraint(solver, b);
	
	if (startEncoding(solver)==1)
		printf("e1=%llu e2=%llu\n", getElementValue(solver,e1), getElementValue(solver, e2));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}
