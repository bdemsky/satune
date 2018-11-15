#include "csolver.h"
#include "multituner.h"
#include "searchtuner.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("You should specify a tuner: %s <best.tuner>\n", argv[0]);
		exit(-1);
	}
	
	SearchTuner *tuner = new SearchTuner(argv[1]);
	tuner->print();
	delete tuner;
	return 0;
}
