
#include "csolver.h"
/**
 * TotalOrder(5, 1, 4)
 * 5 => 1
 * 1 => 4
 * Result: O(5,1)=0 O(1,4)=0 O(5,4)=0 O(1,5)=1 O(1111,5)=2
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {5, 1, 4};
	Set *s = solver->createSet(0, set1, 3);
	Order *order = solver->createOrder(SATC_TOTAL, s);
	BooleanEdge b1 =  solver->orderConstraint(order, 5, 1);
	BooleanEdge b2 =  solver->orderConstraint(order, 1, 4);

	solver->addConstraint(solver->applyLogicalOperation(SATC_OR, b1, solver->applyLogicalOperation(SATC_NOT, b2)));
	solver->addConstraint(solver->applyLogicalOperation(SATC_OR, b2, solver->applyLogicalOperation(SATC_NOT, b1)));
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
