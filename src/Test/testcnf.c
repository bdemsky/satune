#include "nodeedge.h"
#include <stdio.h>

int main(int numargs, char ** argv) {
	CNF *cnf=createCNF();
	Edge v1=constraintNewVar(cnf);
	Edge v2=constraintNewVar(cnf);
	Edge v3=constraintNewVar(cnf);
	Edge nv1=constraintNegate(v1);
	printCNF(nv1);
	printf("\n");
	Edge nv2=constraintNegate(v2);
	Edge nv3=constraintNegate(v3);
	Edge iff1=constraintIFF(cnf, nv1, v2);
	printCNF(iff1);
	printf("\n");

	Edge iff2=constraintOR2(cnf, constraintAND2(cnf, v2, v3), constraintAND2(cnf, nv2, nv3));
	printCNF(iff2);
	printf("\n");
	Edge iff3=constraintIFF(cnf, v3, nv1);
	Edge cand=constraintAND(cnf, 3, (Edge[]) {iff1, iff2, iff3});

	printCNF(cand);
	printf("\n");
	addConstraint(cnf, cand);
	int value=solveCNF(cnf);
	if (value==1) {
		bool v1v=getValueCNF(cnf, v1);
		bool v2v=getValueCNF(cnf, v2);
		bool v3v=getValueCNF(cnf, v3);
		printf("%d %u %u %u\n", value, v1v, v2v, v3v);
	} else
		printf("%d\n",value);
	deleteCNF(cnf);
}
