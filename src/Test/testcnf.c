#include "constraint.h"
#include <stdio.h>

int main(int numargs, char ** argv) {
	CNF *cnf=createCNF();
	Edge v1=constraintNewVar(cnf);
	Edge v2=constraintNewVar(cnf);
	Edge v3=constraintNewVar(cnf);
	Edge v4=constraintNewVar(cnf);

	Edge nv1=constraintNegate(v1);
	Edge nv2=constraintNegate(v2);
	Edge nv3=constraintNegate(v3);
	Edge nv4=constraintNegate(v4);

	Edge c1=constraintAND2(cnf, v1, nv2);
	Edge c2=constraintAND2(cnf, v3, nv4);
	Edge c3=constraintAND2(cnf, nv1, v2);
	Edge c4=constraintAND2(cnf, nv3, v4);
	Edge cor=constraintOR2(cnf, constraintAND2(cnf, c1, c2), constraintAND2(cnf, c3, c4));
	printCNF(cor);
	printf("\n");
	addConstraintCNF(cnf, cor);
	int value=solveCNF(cnf);
	if (value==1) {
		bool v1v=getValueCNF(cnf, v1);
		bool v2v=getValueCNF(cnf, v2);
		bool v3v=getValueCNF(cnf, v3);
		bool v4v=getValueCNF(cnf, v4);
		printf("%d %u %u %u %u\n", value, v1v, v2v, v3v, v4v);
	} else
		printf("%d\n",value);
	deleteCNF(cnf);
}
