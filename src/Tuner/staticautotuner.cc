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
	if( result == EXIT_FAILURE) {
		return newTuner;
	}else {
		delete newTuner;
		return NULL;
	}
}

void StaticAutoTuner::tune() {
	StaticSearchTuner *oldTuner = new StaticSearchTuner();
	evaluateAll(oldTuner);
	while (true) {
		StaticSearchTuner *newTuner = mutateTuner(oldTuner);
		if (newTuner == NULL){
			break;
		}
		double newScore = evaluateAll(newTuner);
		newTuner->printUsed();
		model_print("Received score %f\n", newScore);
		delete oldTuner;
		oldTuner = newTuner;
	}
	delete oldTuner;
}
