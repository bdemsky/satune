#include "csolver.h"


InterpreterType getInterpreterType(char * itype){
	if(strcmp (itype,"--alloy") == 0){
		return ALLOY;
	} else if(strcmp (itype,"--z3") == 0){
		return Z3;
	} else if(strcmp (itype,"--smtrat") == 0){
		return SMTRAT;
	} else if(strcmp (itype,"--alloy") == 0){
		return ALLOY;
	} else {
		printf("Unknown interpreter type: %s\n", itype);
		printf("./run.sh deserializer test.dump [--alloy/--z3/--smtrat/--mathsmt]\n");
		exit(-1);
	}
}

int main(int argc, char **argv) {
	printf("%d\n", argc);
	if (argc != 2 && argc != 3) {
		printf("You only specify the name of the file ...\n");
		printf("./run.sh deserializer test.dump [--alloy/--z3/--smtrat/--mathsmt]\n");
		exit(-1);
	}
	CSolver *solver; 
	if(argc == 3){
		solver = CSolver::deserialize(argv[1], getInterpreterType(argv[2]));
	} else {
		solver = CSolver::deserialize(argv[1]);
	}
	int value = solver->solve();
	if (value == 1) {
		printf("%s is SAT\n", argv[1]);
	} else {
		printf("%s is UNSAT\n", argv[1]);
	}
	delete solver;
	return 1;

}
