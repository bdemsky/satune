#include "csolver.h"

int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {1, 2, 3, 4, 5, 6, 7, 8};
	Set *s = solver->createSet(0, set1, 8);
	Order *order = solver->createOrder(SATC_TOTAL, s);
	BooleanEdge o12 =  solver->orderConstraint(order, 1, 2);
	BooleanEdge o13 =  solver->orderConstraint(order, 1, 3);
	BooleanEdge o24 =  solver->orderConstraint(order, 2, 4);
	BooleanEdge o34 =  solver->orderConstraint(order, 3, 4);
	BooleanEdge o41 =  solver->orderConstraint(order, 4, 1);
	BooleanEdge o57 =  solver->orderConstraint(order, 5, 7);
	BooleanEdge o76 =  solver->orderConstraint(order, 7, 6);
	BooleanEdge o65 =  solver->orderConstraint(order, 6, 5);
	BooleanEdge o58 =  solver->orderConstraint(order, 5, 8);
	BooleanEdge o81 =  solver->orderConstraint(order, 8, 1);

	BooleanEdge array1[] = {o12, o13, o24, o34};
	solver->addConstraint(solver->applyLogicalOperation(SATC_OR, array1, 4) );
	BooleanEdge array2[] = {o41, o57};

	BooleanEdge b1 = solver->applyLogicalOperation(SATC_XOR, array2, 2);
	BooleanEdge array3[] = {o34};
	BooleanEdge o34n = solver->applyLogicalOperation(SATC_NOT, array3, 1);
	BooleanEdge array4[] = {o24};
	BooleanEdge o24n = solver->applyLogicalOperation(SATC_NOT, array4, 1);
	BooleanEdge array5[] = {o34n, o24n};
	BooleanEdge b2 = solver->applyLogicalOperation(SATC_OR, array5, 2);
	BooleanEdge array6[] = {b1, b2};
	solver->addConstraint(solver->applyLogicalOperation(SATC_IMPLIES, array6, 2) );

	BooleanEdge array7[] = {o12, o13};
	solver->addConstraint(solver->applyLogicalOperation(SATC_AND, array7, 2) );

	BooleanEdge array8[] = {o76, o65};
	solver->addConstraint(solver->applyLogicalOperation(SATC_OR, array8, 2) );

	BooleanEdge array9[] = {o76, o65};
	BooleanEdge b3 = solver->applyLogicalOperation(SATC_AND, array9, 2) ;
	BooleanEdge array10[] = {o57};
	BooleanEdge o57n = solver->applyLogicalOperation(SATC_NOT, array10, 1);
	BooleanEdge array11[] = {b3, o57n};
	solver->addConstraint(solver->applyLogicalOperation(SATC_IMPLIES, array11, 2));

	BooleanEdge array12[] = {o58, o81};
	solver->addConstraint(solver->applyLogicalOperation(SATC_AND, array12, 2) );
	solver->serialize();
	/*	if (solver->solve() == 1)
	   printf("SAT\n");
	   else
	   printf("UNSAT\n");*/

	solver->autoTune(100);
	delete solver;
}
