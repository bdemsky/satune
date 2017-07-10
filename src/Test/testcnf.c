#include "nodeedge.h"

int main(int numargs, char ** argv) {
	CNF *cnf=createCNF();
	Edge v1=constraintNewVar(cnf);
	Edge v2=constraintNewVar(cnf);
	Edge v3=constraintNewVar(cnf);
	Edge nv1=constraintNegate(v1);
	Edge nv2=constraintNegate(v2);
	Edge iff1=constraintIFF(cnf, nv1, v2);
	Edge iff2=constraintIFF(cnf, nv2, v3);
	Edge iff3=constraintIFF(cnf, v3, nv1);
	Edge cand=constraintAND(cnf, 3, (Edge[]) {iff1, iff2, iff3});
	addConstraint(cnf, cand);
	solveCNF(cnf);
	deleteCNF(cnf);
}
