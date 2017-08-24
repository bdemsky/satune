#include "csolver.h"

int main(int numargs, char **argv) {
	CSolver *solver = allocCSolver();
	uint64_t set1[] = {1, 2, 3, 4, 5, 6, 7, 8};
	Set *s = createSet(solver, 0, set1, 8);
	Order *order = createOrder(solver, TOTAL, s);
	Boolean *o12 =  orderConstraint(solver, order, 1, 2);
	Boolean *o13 =  orderConstraint(solver, order, 1, 3);
	Boolean *o24 =  orderConstraint(solver, order, 2, 4);
	Boolean *o34 =  orderConstraint(solver, order, 3, 4);
	Boolean *o41 =  orderConstraint(solver, order, 4, 1);
	Boolean *o57 =  orderConstraint(solver, order, 5, 7);
	Boolean *o76 =  orderConstraint(solver, order, 7, 6);
	Boolean *o65 =  orderConstraint(solver, order, 6, 5);
	Boolean *o58 =  orderConstraint(solver, order, 5, 8);
	Boolean *o81 =  orderConstraint(solver, order, 8, 1);
	
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
	
	if (startEncoding(solver) == 1)
		printf("SAT\n");
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}