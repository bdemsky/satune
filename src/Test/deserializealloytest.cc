#include "csolver.h"


int main(int argc, char **argv) {
	printf("%d\n", argc);
	if (argc != 2 && argc != 3) {
		printf("You only specify the name of the file ...\n");
		printf("./run.sh deserializer test.dump [--alloy]\n");
		exit(-1);
	}
	CSolver *solver = CSolver::deserialize(argv[1]);
	if(argc == 3)
		solver->setAlloyEncode();
	int value = solver->solve();
	if (value == 1) {
		printf("%s is SAT\n", argv[1]);
	} else {
		printf("%s is UNSAT\n", argv[1]);
	}
	delete solver;
	return 1;

}
