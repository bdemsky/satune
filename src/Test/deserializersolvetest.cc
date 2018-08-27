#include "csolver.h"
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("You should specify file names ...");
		exit(-1);
	}
	//usleep(20000000);
	for (int i = 1; i < argc; i++) {
		CSolver *solver = CSolver::deserialize(argv[i]);
		int value = solver->solve();
		if (value == 1) {
			printf("%s is SAT\n", argv[i]);
		} else {
			printf("%s is UNSAT\n", argv[i]);
		}
		delete solver;
	}
	return 1;
}
