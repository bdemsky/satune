
#include "csolver.h"
/**
 * TotalOrder(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
 * 0=>1
 * 1=>2
 * 2=>3
 * 1=>4
 * 4=>5
 * 5=>6
 * 1=>7
 * 7=>8
 * 8=>9
 * 9=>2
 * 6=>2
 *
 *
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	Set *s = solver->createSet(0, set1, 9);
	Order *order = solver->createOrder(SATC_TOTAL, s);
	BooleanEdge b12 =  solver->orderConstraint(order, 1, 2);
	solver->addConstraint(b12);
	BooleanEdge b23 =  solver->orderConstraint(order, 2, 3);
	solver->addConstraint(b23);
	BooleanEdge b14 =  solver->orderConstraint(order, 1, 4);
	solver->addConstraint(b14);
	BooleanEdge b45 =  solver->orderConstraint(order, 4, 5);
	solver->addConstraint(b45);
	BooleanEdge b56 =  solver->orderConstraint(order, 5, 6);
	solver->addConstraint(b56);
	BooleanEdge b17 =  solver->orderConstraint(order, 1, 7);
	solver->addConstraint(b17);
	BooleanEdge b78 =  solver->orderConstraint(order, 7, 8);
	solver->addConstraint(b78);
	BooleanEdge b89 =  solver->orderConstraint(order, 8, 9);
	solver->addConstraint(b89);
	BooleanEdge b92 =  solver->orderConstraint(order, 9, 2);
	solver->addConstraint(b92);
	BooleanEdge b62 =  solver->orderConstraint(order, 6, 2);
	solver->addConstraint(b62);

	BooleanEdge v6 = solver->getBooleanVar(0);
	BooleanEdge v7 = solver->getBooleanVar(0);
	BooleanEdge v8 = solver->getBooleanVar(0);
	solver->addConstraint(solver->applyLogicalOperation(SATC_OR, solver->applyLogicalOperation(SATC_IFF, v7, v8), v6));
	solver->addConstraint(solver->applyLogicalOperation(SATC_NOT, v6));
	solver->addConstraint(solver->applyLogicalOperation(SATC_NOT, v8));
	BooleanEdge b48 =  solver->orderConstraint(order, 4, 8);
	solver->addConstraint(b48);
	BooleanEdge b57 =  solver->orderConstraint(order, 5, 7);
	solver->addConstraint(b57);


	if (solver->solve() == 1) {
		printf("SAT\n");
		printf("O(5,1)=%d O(1,4)=%d O(5,4)=%d O(1,5)=%d\n",
					 solver->getOrderConstraintValue(order, 5, 1),
					 solver->getOrderConstraintValue(order, 1, 4),
					 solver->getOrderConstraintValue(order, 5, 4),
					 solver->getOrderConstraintValue(order, 1, 5));
	} else {
		printf("UNSAT\n");
	}
	delete solver;
}
