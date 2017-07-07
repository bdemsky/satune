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
	Order * o=createOrder(solver, TOTAL, s);
	Boolean * oc=orderConstraint(solver, o, 1, 2);
	addBoolean(solver, oc);
	
	uint64_t set2[] = {2, 3};
	Set* range = createSet(solver, 1, set2, 2);
	Function * f1 = createFunctionOperator(solver, ADD, domain, 2, range, IGNORE);
	/*Table* table = createTable(solver, domain, 2, range);
	uint64_t row1[] = {0, 1};
	uint64_t row2[] = {1, 1};
	addTableEntry(solver, table, row1, 2, 2);
	addTableEntry(solver, table, row2, 2, 3);
	Function * f2 = completeTable(solver, table); */
	Boolean* overflow = getBooleanVar(solver , 2);
	Element * e3 = applyFunction(solver, f1, inputs, 2, overflow);
	Set* domain2[] = {s,range};
	Predicate* equal2 = createPredicateOperator(solver, EQUALS, domain2, 2);
	Element* inputs2 [] = {e1, e3};
	Boolean* pred = applyPredicate(solver, equal2, inputs2, 2);
	addBoolean(solver, pred);
	startEncoding(solver);
	deleteSolver(solver);
}
