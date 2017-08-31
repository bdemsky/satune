#include "csolver.h"

/**
 * b1 AND b2=>b3
 * !b3 OR b4
 * b1 XOR b4
 * Result: b1=1 b2=0 b3=0 b4=0
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	Boolean *b1 = solver->getBooleanVar(0);
	Boolean *b2 = solver->getBooleanVar(0);
	Boolean *b3 = solver->getBooleanVar(0);
	Boolean *b4 = solver->getBooleanVar(0);
	//SATC_AND, SATC_OR, SATC_NOT, SATC_XOR, SATC_IMPLIES
	Boolean *barray1[] = {b1,b2};
	Boolean *andb1b2 = solver->applyLogicalOperation(SATC_AND, barray1, 2);
	Boolean *barray2[] = {andb1b2, b3};
	Boolean *imply = solver->applyLogicalOperation(SATC_IMPLIES, barray2, 2);
	solver->addConstraint(imply);
	Boolean *barray3[] = {b3};
	Boolean *notb3 = solver->applyLogicalOperation(SATC_NOT, barray3, 1);
	Boolean *barray4[] = {notb3, b4};
	solver->addConstraint(solver->applyLogicalOperation(SATC_OR, barray4, 2));
	Boolean *barray5[] = {b1, b4};
	solver->addConstraint(solver->applyLogicalOperation(SATC_XOR, barray5, 2));
	if (solver->startEncoding() == 1)
		printf("b1=%d b2=%d b3=%d b4=%d\n",
					 solver->getBooleanValue(b1), solver->getBooleanValue(b2),
					 solver->getBooleanValue(b3), solver->getBooleanValue(b4));
	else
		printf("UNSAT\n");
	delete solver;
}
