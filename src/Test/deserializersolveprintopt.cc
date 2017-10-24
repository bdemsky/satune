#include "csolver.h"


int main(int argc, char ** argv){
	if(argc < 2){
		printf("You should specify file names ...");
		exit(-1);	
	}
	for(int i = 1; i < argc; i++) {
		CSolver* solver = CSolver::deserialize(argv[i]);
		int value=solver->solve();
		if (value ==1) {
			printf("%s is SAT\n", argv[i]);
		} else {
			printf("%s is UNSAT\n", argv[i]);
		}
		solver->printConstraints();

		delete solver;
	}
	return 1;
}