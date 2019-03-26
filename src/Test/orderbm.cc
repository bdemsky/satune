#include <stdlib.h>
#include "csolver.h"
/**
 * TotalOrder(5, 1, 4)
 * 5 => 1
 * 1 => 4
 * Result: O(5,1)=0 O(1,4)=0 O(5,4)=0 O(1,5)=1 O(1111,5)=2
 */
int main(int numargs, char **argv) {
	if (numargs < 4) {
		printf("Requires the following arguments: numpoints numclauses maxclausesize randomseed\n");
		return -1;
	}
	int numpoints = atoi(argv[1]);
	int numclauses = atoi(argv[2]);
	int maxclause = atoi(argv[3]);
	srandom(atoi(argv[4]));

	CSolver *solver = new CSolver();
	Set *s = solver->createRangeSet(0, 0, numpoints);
	Order *order = solver->createOrder(SATC_TOTAL, s);
	BooleanEdge be[maxclause];
	for (int i = 0; i < numclauses; i++) {
		int numterms = (random() % (maxclause - 1)) + 1;

		for (int j = 0; j < numterms; j++) {
			uint src = random() % (numpoints - 1);
			uint dst;
			do {
				dst = random() % numpoints;
			} while (src == dst || ((false) && (src > dst)));

			be[j] =  solver->orderConstraint(order, src, dst);
		}
		solver->addConstraint(solver->applyLogicalOperation(SATC_OR, be, numterms));
	}
	solver->serialize();
	if (solver->solve() == 1) {
		printf("SAT\n");
	} else {
		printf("UNSAT\n");
	}
	delete solver;
}
