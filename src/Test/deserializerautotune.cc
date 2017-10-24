#include "csolver.h"
#include "autotuner.h"

int main(int argc, char ** argv){
	if(argc < 2){
		printf("You should specify file names ...");
		exit(-1);	
	}
	CSolver * solvers[argc-1];
	AutoTuner *autotuner = new AutoTuner(100);
	for(int i = 1; i < argc; i++) {
		solvers[i-1] = CSolver::deserialize(argv[i]);
		autotuner->addProblem(solvers[i-1]);
	}

	autotuner->tune();
	delete autotuner;
	
	for(int i = 1; i < argc; i++) {
		delete solvers[i-1];
	}

	return 1;
}
