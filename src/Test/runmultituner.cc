#include "csolver.h"
#include "searchtuner.h"
#include "kmeanstuner.h"
#include "satuner.h"
#include "comptuner.h"
#include "randomtuner.h"

void printKnownTunerTypes(){
	printf("Known Tuner Types:\nRandom Tuner=1\nComp Tuner=2\nKmeans Tuner=3\nSimulated Annealing Tuner=4\n");
}

BasicTuner *createTuner(uint tunertype, uint budget, uint rounds, uint timeout){
	switch(tunertype){
		case 1: return new RandomTuner(budget, timeout);
		case 2: return new CompTuner(budget, timeout);
		case 3: return new KMeansTuner(budget, rounds, timeout);
		case 4: return new SATuner(budget, timeout);
		default:
			printf("Tuner type %u is unknown\n", tunertype);
			printKnownTunerTypes();
			exit(-1);
	}

}

int main(int argc, char **argv) {
	if (argc < 8) {
		printf("You should specify: %s TunerType budget rounds timeout problemfilenames - tunerfilenames\n", argv[0]);
		printKnownTunerTypes();
		exit(-1);
	}
	uint tunertype;
	uint budget;
	uint rounds;
	uint timeout;
	sscanf(argv[1], "%u", &tunertype);
	sscanf(argv[2], "%u", &budget);
	sscanf(argv[3], "%u", &rounds);
	sscanf(argv[4], "%u", &timeout);

	BasicTuner *multituner = createTuner(tunertype, budget, rounds, timeout);
	bool tunerfiles = false;
	for (int i = 5; i < argc; i++) {
		if (!tunerfiles) {
			if (argv[i][0] == '-' && argv[i][1] == 0)
				tunerfiles = true;
			else
				multituner->addProblem(argv[i]);
		} else
			multituner->addTuner(new SearchTuner(argv[i], true )); //add settings to usedsettigs
	}

	if (!tunerfiles) {
		printf("You should specify %s budget rounds timeout problemfilenames - tunerfilenames", argv[0]);
		exit(-1);
	}

	multituner->tune();
	delete multituner;
	return 0;
}
