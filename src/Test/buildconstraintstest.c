#include "csolver.h"

int main(int numargs, char ** argv) {
	CSolver * solver=allocCSolver();
	uint64_t set1[]={0, 1, 2};
	uint64_t setbigarray[]={0, 1, 2, 3, 4};
	
	Set * s=createSet(solver, 0, set1, 3);
	Set * setbig=createSet(solver, 0, setbigarray, 5);
	Element * e1=getElementVar(solver, s);
	Element * e2=getElementVar(solver, s);
	Set * domain[]={s, s};
	Predicate *equals=createPredicateOperator(solver, EQUALS, domain, 2);
	Element * inputs[]={e1, e2};
	Boolean* overflow = getBooleanVar(solver , 2);
	Boolean * b=applyPredicate(solver, equals, inputs, 2, overflow);
	addConstraint(solver, b);

	uint64_t set2[] = {2, 3};
	Set* rangef1 = createSet(solver, 1, set2, 2);
	Function * f1 = createFunctionOperator(solver, ADD, domain, 2, setbig, IGNORE);
	
	Table* table = createTable(solver, domain, 2, s);
	uint64_t row1[] = {0, 1};
	uint64_t row2[] = {1, 1};
	uint64_t row3[] = {2, 1};
	uint64_t row4[] = {2, 2};
	addTableEntry(solver, table, row1, 2, 0);
	addTableEntry(solver, table, row2, 2, 0);
	addTableEntry(solver, table, row3, 2, 2);
	addTableEntry(solver, table, row4, 2, 2);
	Function * f2 = completeTable(solver, table, IGNOREBEHAVIOR); //its range would be as same as s
	
	Element * e3 = applyFunction(solver, f1, inputs, 2, overflow);
	Element * e4 = applyFunction(solver, f2, inputs, 2, overflow);
	Set* domain2[] = {s,rangef1};
	Predicate* equal2 = createPredicateOperator(solver, EQUALS, domain2, 2);
	Element* inputs2 [] = {e4, e3};
	Boolean* pred = applyPredicate(solver, equal2, inputs2, 2, overflow);
	addConstraint(solver, pred);
	
	if (startEncoding(solver)==1)
		printf("e1=%llu e2=%llu\n", getElementValue(solver,e1), getElementValue(solver, e2));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}
