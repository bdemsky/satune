#include "csolver.h"

/**
 * !b1 AND
 * !b2 OR b1 or (!b3 and b4) AND
 * b7 OR (!b1 AND (b5 or !b6))
 *
 */
int main(int numargs, char **argv) {
	CSolver *solver = new CSolver();
	BooleanEdge b1 = solver->getBooleanVar(0);
	BooleanEdge b2 = solver->getBooleanVar(0);
	BooleanEdge b3 = solver->getBooleanVar(0);
	BooleanEdge b4 = solver->getBooleanVar(0);
	BooleanEdge b5 = solver->getBooleanVar(0);
	BooleanEdge b6 = solver->getBooleanVar(0);
	BooleanEdge b7 = solver->getBooleanVar(0);
	//SATC_AND, SATC_OR, SATC_NOT, SATC_XOR, SATC_IMPLIES
	BooleanEdge in[] = {b1};
	BooleanEdge notb1 = solver->applyLogicalOperation(SATC_NOT, in, 1);
	BooleanEdge in2[] = {b2};
	BooleanEdge notb2 = solver->applyLogicalOperation(SATC_NOT, in2, 1);
	BooleanEdge in3[] = {b3};
	BooleanEdge notb3 = solver->applyLogicalOperation(SATC_NOT, in3, 1);
	BooleanEdge in4[] = {notb3,b4};
	BooleanEdge andnotb3b4 = solver->applyLogicalOperation(SATC_AND, in4, 2);
	BooleanEdge in5[] = {notb2, b1, andnotb3b4};
	BooleanEdge secondc = solver->applyLogicalOperation(SATC_OR, in5, 3);
	BooleanEdge in6[] = {b6};
	BooleanEdge notb6 = solver->applyLogicalOperation(SATC_NOT, in6, 1);
	BooleanEdge in7[] = {b5,notb6};
	BooleanEdge orb5notb6 = solver->applyLogicalOperation(SATC_OR, in7, 2);
	BooleanEdge in8[] = {notb1,orb5notb6};
	BooleanEdge andnotb1ors = solver->applyLogicalOperation(SATC_AND, in8, 2);
	BooleanEdge in9[] = {b7, andnotb1ors};
	BooleanEdge third = solver->applyLogicalOperation(SATC_OR, in9, 2);
	BooleanEdge in10[] = {secondc, third, notb1};
	BooleanEdge final = solver->applyLogicalOperation(SATC_AND, in10, 3);
	solver->addConstraint(final);
	printf("&&&&&&&&&&&&&&&&&&&&&&&\n");
	solver->printConstraints();
	if (solver->solve() == 1)
		printf("b1=%d\nb2=%d\nb3=%d\nb4=%d\nb5=%d\nb6=%d\nb7=%d\n",
					 solver->getBooleanValue(b1), solver->getBooleanValue(b2),
					 solver->getBooleanValue(b3), solver->getBooleanValue(b4),
					 solver->getBooleanValue(b5), solver->getBooleanValue(b6),
					 solver->getBooleanValue(b7));
	else
		printf("UNSAT\n");
	delete solver;
}
