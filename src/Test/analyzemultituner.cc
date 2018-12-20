#include "csolver.h"
#include "comptuner.h"
#include "searchtuner.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("You should specify number of runs\n");
		exit(-1);
	}
	uint numruns;
	sscanf(argv[1], "%u", &numruns);

	CompTuner *multituner = new CompTuner(0, 0);
	multituner->readData(numruns);
	multituner->findBestThreeTuners();
	delete multituner;
	return 0;
}
