#include "csolver.h"

int main(int numargs, char ** argv) {
	CSolver * solver=allocCSolver();
	uint64_t set1[]={0, 1, 2};
	Set * s=createSet(solver, 0, set1, 3);
	Element * e1=getElementVar(solver, s);
	Element * e2=getElementVar(solver, s);
	Set * domain[]={s, s};
	Predicate *equals=createPredicateOperator(solver, EQUALS, domain, 2);
	Element * inputs[]={e1, e2};
	Boolean * b=applyPredicate(solver, equals, inputs, 2);
	addBoolean(solver, b);
	deleteSolver(solver);
}
