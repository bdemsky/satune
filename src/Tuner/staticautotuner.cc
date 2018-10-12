#include "staticautotuner.h"
#include "csolver.h"
#include "staticsearchtuner.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define UNSETVALUE -1
#define TIMEOUTSEC 5000
StaticAutoTuner::StaticAutoTuner(uint _budget) : AutoTuner(_budget) {
}

StaticSearchTuner *StaticAutoTuner::mutateTuner(StaticSearchTuner *oldTuner) {
	StaticSearchTuner *newTuner = oldTuner->copyUsed();
	result = newTuner->nextStaticTuner();
	return result == EXIT_FAILURE ? newTuner : NULL;
}

void StaticAutoTuner::tune() {
	StaticSearchTuner *bestTuner = NULL;
	double bestScore = DBL_MAX;

	StaticSearchTuner *oldTuner = new StaticSearchTuner();
	double base_temperature = evaluateAll(oldTuner);
	double oldScore = base_temperature;

	while (true) {
		StaticSearchTuner *newTuner = mutateTuner(oldTuner);
		if (newTuner == NULL)
			return;
		double newScore = evaluateAll(newTuner);
		newTuner->printUsed();
		model_print("Received score %f\n", newScore);
		delete oldTuner;
		oldScore = newScore;
		oldTuner = newTuner;
	}
}
