#include "csolver.h"
#include "randomtuner.h"
#include "searchtuner.h"

int main(int argc, char **argv) {
	if (argc < 6) {
		printf("You should specify %s rounds timeout problemfilenames - tunerfilenames", argv[0]);
		exit(-1);
	}
	uint rounds;
	uint timeout;
	sscanf(argv[1], "%u", &rounds);
	sscanf(argv[2], "%u", &timeout);

	RandomTuner *randomTuner = new RandomTuner(rounds, timeout);
	bool tunerfiles = false;
	for (int i = 3; i < argc; i++) {
		if (!tunerfiles) {
			if (argv[i][0] == '-' && argv[i][1] == 0)
				tunerfiles = true;
			else
				randomTuner->addProblem(argv[i]);
		} else
			randomTuner->addTuner(new SearchTuner(argv[i]));
	}

	if (!tunerfiles) {
		printf("You should specify %s budget rounds timeout problemfilenames - tunerfilenames", argv[0]);
		exit(-1);
	}

	randomTuner->tune();
	delete randomTuner;
	return 0;
}
