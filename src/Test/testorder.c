
#include "csolver.h"

int main(int numargs, char ** argv) {
	CSolver * solver=allocCSolver();
	uint64_t set1[]={5, 1, 4};
	Set * s=createSet(solver, 0, set1, 3);
	Order* order = createOrder(solver, TOTAL, s);
	Boolean* b1=  orderConstraint(solver, order, 5, 1);
	Boolean* b2=  orderConstraint(solver, order, 1, 4);
	addConstraint(solver, b1);
	addConstraint(solver, b2);
	if (startEncoding(solver)==1)
		printf("O(5,1)=%d O(1,4)=%d O(5,4)=%d O(1,5)=%d O(1111,5)=%d\n", 
			getOrderConstraintValue(solver, order, 5, 1), 
			getOrderConstraintValue(solver, order, 1, 4),
			getOrderConstraintValue(solver, order, 5, 4),
			getOrderConstraintValue(solver, order, 1, 5),
			getOrderConstraintValue(solver, order, 1111, 5));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}