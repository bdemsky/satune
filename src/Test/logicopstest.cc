#include "csolver.h"

/**
 * b1 AND b2=>b3
 * !b3 OR b4
 * b1 XOR b4
 * Result: b1=1 b2=0 b3=0 b4=0
 */
int main(int numargs, char **argv) {
	CSolver *solver = allocCSolver();
	Boolean *b1 = getBooleanVar(solver, 0);
	Boolean *b2 = getBooleanVar(solver, 0);
	Boolean *b3 = getBooleanVar(solver, 0);
	Boolean *b4 = getBooleanVar(solver, 0);
	//L_AND, L_OR, L_NOT, L_XOR, L_IMPLIES
	Boolean * barray1[]={b1,b2};
	Boolean *andb1b2 = applyLogicalOperation(solver, L_AND, barray1, 2);
	Boolean * barray2[]={andb1b2, b3};
	Boolean *imply = applyLogicalOperation(solver, L_IMPLIES, barray2, 2);
	addConstraint(solver, imply);
	Boolean * barray3[] ={b3};
	Boolean *notb3 = applyLogicalOperation(solver, L_NOT, barray3, 1);
	Boolean * barray4[] ={notb3, b4};
	addConstraint(solver, applyLogicalOperation(solver, L_OR, barray4, 2));
	Boolean * barray5[] ={b1, b4};
	addConstraint(solver, applyLogicalOperation(solver, L_XOR, barray5, 2));
	if (startEncoding(solver) == 1)
		printf("b1=%d b2=%d b3=%d b4=%d\n",
					 getBooleanValue(solver,b1), getBooleanValue(solver, b2),
					 getBooleanValue(solver, b3), getBooleanValue(solver, b4));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}
