#include "csolver.h"

int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {1, 2, 3, 4, 5, 6, 7, 8};
	Set *s = solver->createSet(0, set1, 8);
	Order *order = solver->createOrder(TOTAL, s);
	Boolean *o12 =  solver->orderConstraint(order, 1, 2);
	Boolean *o13 =  solver->orderConstraint(order, 1, 3);
	Boolean *o24 =  solver->orderConstraint(order, 2, 4);
	Boolean *o34 =  solver->orderConstraint(order, 3, 4);
	Boolean *o41 =  solver->orderConstraint(order, 4, 1);
	Boolean *o57 =  solver->orderConstraint(order, 5, 7);
	Boolean *o76 =  solver->orderConstraint(order, 7, 6);
	Boolean *o65 =  solver->orderConstraint(order, 6, 5);
	Boolean *o58 =  solver->orderConstraint(order, 5, 8);
	Boolean *o81 =  solver->orderConstraint(order, 8, 1);

	/* Not Valid c++...Let Hamed fix...
	addConstraint(solver, applyLogicalOperation(solver, L_OR,(Boolean *[]) {o12, o13, o24, o34}, 4) );
	Boolean *b1 = applyLogicalOperation(solver, L_XOR, (Boolean *[]) {o41, o57}, 2);
	Boolean *o34n = applyLogicalOperation(solver, L_NOT, (Boolean *[]) {o34}, 1);
	Boolean *o24n = applyLogicalOperation(solver, L_NOT, (Boolean *[]) {o24}, 1);
	Boolean *b2 = applyLogicalOperation(solver, L_OR, (Boolean *[]) {o34n, o24n}, 2);
	addConstraint(solver, applyLogicalOperation(solver, L_IMPLIES,(Boolean *[]) {b1, b2}, 2) );
	addConstraint(solver, applyLogicalOperation(solver, L_AND,(Boolean *[]) {o12, o13}, 2) );
	addConstraint(solver, applyLogicalOperation(solver, L_OR,(Boolean *[]) {o76, o65}, 2) );
	Boolean* b3= applyLogicalOperation(solver, L_AND,(Boolean *[]) {o76, o65}, 2) ;
	Boolean* o57n= applyLogicalOperation(solver, L_NOT,(Boolean *[]) {o57}, 1);
	addConstraint(solver, applyLogicalOperation(solver, L_IMPLIES,(Boolean *[]) {b3, o57n}, 2) );
	addConstraint(solver, applyLogicalOperation(solver, L_AND,(Boolean *[]) {o58, o81}, 2) );
	
	if (solver->startEncoding() == 1)
		printf("SAT\n");
	else
	printf("UNSAT\n");
	*/
	delete solver;
}
