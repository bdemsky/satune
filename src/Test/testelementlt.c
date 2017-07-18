#include "csolver.h"

int main(int numargs, char ** argv){
	CSolver *solver=allocCSolver();
	uint64_t set1[]={0, 1, 2};
	uint64_t set3[]={1, 3, 4, 5};
	Set * s1=createSet(solver, 0, set1, 3);
	Set * s3=createSet(solver, 0, set3, 4);
	Element * e1=getElementVar(solver, s1);
	Element * e3=getElementVar(solver, s3);
	Set * domain2[]={s1, s3};
	Predicate *lt=createPredicateOperator(solver, LT, domain2, 2);
	Element * inputs2[]={e1, e3};
	Boolean *b=applyPredicate(solver, lt, inputs2, 2);
	addConstraint(solver, b);
	if (startEncoding(solver)==1)
		printf("e1=%llu e3=%llu\n", getElementValue(solver,e1), getElementValue(solver, e3));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}