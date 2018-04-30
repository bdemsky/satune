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
		CSolver *copy = solver->clone();
                CSolver *copy2 = solver->clone();
                CSolver *copy3 = solver->clone();
                CSolver *copy4 = solver->clone();
                int value = copy->solve();
                if (value == 1) {
			printf("Copy %s is SAT\n", argv[i]);
		} else {
			printf("Copy %s is UNSAT\n", argv[i]);
		}
                value = copy2->solve();
                if (value == 1) {
			printf("Copy2 %s is SAT\n", argv[i]);
		} else {
			printf("Copy2 %s is UNSAT\n", argv[i]);
		}
                value = copy3->solve();
                if (value == 1) {
			printf("Copy3 %s is SAT\n", argv[i]);
		} else {
			printf("Copy3 %s is UNSAT\n", argv[i]);
		}
                value = copy4->solve();
                if (value == 1) {
			printf("Copy4 %s is SAT\n", argv[i]);
		} else {
			printf("Copy4 %s is UNSAT\n", argv[i]);
		}
                value = solver->solve();
		if (value == 1) {
			printf("Original %s is SAT\n", argv[i]);
		} else {
			printf("Original %s is UNSAT\n", argv[i]);
		}
                
		delete solver;
	}
	return 1;
}
