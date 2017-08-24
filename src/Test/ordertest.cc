
#include "csolver.h"
/**
 * TotalOrder(5, 1, 4)
 * 5 => 1
 * 1 => 4
 * Result: O(5,1)=0 O(1,4)=0 O(5,4)=0 O(1,5)=1 O(1111,5)=2
 */
int main(int numargs, char **argv) {
	CSolver *solver = allocCSolver();
	uint64_t set1[] = {5, 1, 4};
	Set *s = createSet(solver, 0, set1, 3);
	Order *order = createOrder(solver, TOTAL, s);
	Boolean *b1 =  orderConstraint(solver, order, 5, 1);
	Boolean *b2 =  orderConstraint(solver, order, 1, 4);
	addConstraint(solver, b1);
	addConstraint(solver, b2);
	if (startEncoding(solver) == 1)
		printf("SAT\n");
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}
