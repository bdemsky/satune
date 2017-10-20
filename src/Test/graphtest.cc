#include "csolver.h"

int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	uint64_t set1[] = {1, 2, 3};
	Set *s = solver->createSet(0, set1, 3);
	Order *order = solver->createOrder(SATC_TOTAL, s);
	BooleanEdge b12 =  solver->orderConstraint(order, 1, 2);
	solver->addConstraint(b12);
	BooleanEdge b23 =  solver->orderConstraint(order, 2, 3);
	solver->addConstraint(b23);

	if (solver->solve() == 1) {
		printf("SAT\n");
	} else {
		printf("UNSAT\n");
	}
	delete solver;
}
