#include "csolver.h"

int main(int numargs, char ** argv) {
	CSolver * solver=allocCSolver();
	uint64_t set1[]={0, 1, 2};
	uint64_t set2[]={3, 4};
	Set * s1=createSet(solver, 0, set1, 3);
	Set * s2=createSet(solver, 0, set2, 2);
	Element * e1=getElementVar(solver, s1);
	Element * e2=getElementVar(solver, s2);
	Set * domain[]={s1, s2};
	Predicate *equals=createPredicateOperator(solver, EQUALS, domain, 2);
	Element * inputs[]={e1, e2};
	Boolean *b=applyPredicate(solver, equals, inputs, 2, NULL);
	addConstraint(solver, b);
	
	if (startEncoding(solver)==1)
		printf("e1=%llu e2=%llu\n", getElementValue(solver,e1), getElementValue(solver, e2));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}
