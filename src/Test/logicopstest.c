#include "csolver.h"

/**
 * b1 AND b2=>b3
 * !b3 OR b4
 * b1 XOR b4
 * @param numargs
 * @param argv
 * @return 
 */
int main(int numargs, char** argv){
	CSolver * solver=allocCSolver();
	Boolean *b1= getBooleanVar(solver, 0);
	Boolean *b2= getBooleanVar(solver, 0);
	Boolean *b3= getBooleanVar(solver, 0);
	Boolean *b4= getBooleanVar(solver, 0);
	//L_AND, L_OR, L_NOT, L_XOR, L_IMPLIES
	Boolean *andb1b2= applyLogicalOperation(solver, L_AND,(Boolean*[]) {b1, b2}, 2);
	Boolean * imply = applyLogicalOperation(solver, L_IMPLIES, (Boolean*[]) {andb1b2, b3}, 2);
	addConstraint(solver, imply);
	Boolean* notb3 = applyLogicalOperation(solver, L_NOT, (Boolean*[]) {b3}, 1); 
	addConstraint(solver, applyLogicalOperation(solver, L_OR, (Boolean*[]){notb3, b4} , 2));
	addConstraint(solver, applyLogicalOperation(solver, L_XOR, (Boolean* []) {b1, b4}, 2));
	if (startEncoding(solver)==1)
		printf("b1=%d b2=%d b3=%d b4=%d\n", 
			getBooleanValue(solver,b1), getBooleanValue(solver, b2),
			getBooleanValue(solver, b3), getBooleanValue(solver, b4));
	else
		printf("UNSAT\n");
	deleteSolver(solver);
}