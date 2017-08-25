
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
	Order *order = solver->createOrder(TOTAL, s);
	Boolean *b1 =  solver->orderConstraint(order, 5, 1);
	Boolean *b2 =  solver->orderConstraint(order, 1, 4);
	solver->addConstraint(b1);
	solver->addConstraint(b2);
	if (solver->startEncoding() == 1)
		printf("SAT\n");
	else
		printf("UNSAT\n");
	delete solver;
}
