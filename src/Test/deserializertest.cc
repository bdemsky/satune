#include "csolver.h"


int main(int argc, char **argv) {
	if (argc != 2) {
		printf("You only specify the name of the file ...");
		exit(-1);
	}
	CSolver *solver = CSolver::deserialize(argv[1]);
	solver->printConstraints();
	delete solver;
	return 1;

}
