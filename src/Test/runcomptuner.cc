#include "csolver.h"
#include "multituner.h"
#include "searchtuner.h"

int main(int argc, char **argv) {
	if (argc < 7) {
		printf("You should specify %s budget rounds timeout problemfilenames - tunerfilenames", argv[0]);
		exit(-1);
	}
	uint budget;
	uint rounds;
	uint timeout;
	sscanf(argv[1], "%u", &budget);
	sscanf(argv[2], "%u", &rounds);
	sscanf(argv[3], "%u", &timeout);

	MultiTuner *multituner = new MultiTuner(budget, rounds, timeout);
	bool tunerfiles = false;
	for (int i = 4; i < argc; i++) {
		if (!tunerfiles) {
			if (argv[i][0] == '-' && argv[i][1] == 0)
				tunerfiles = true;
			else
				multituner->addProblem(argv[i]);
		} else
			multituner->addTuner(new SearchTuner(argv[i]));
	}

	if (!tunerfiles) {
		printf("You should specify %s budget rounds timeout problemfilenames - tunerfilenames", argv[0]);
		exit(-1);
	}

	multituner->tuneComp();
	delete multituner;
	return 0;
}
