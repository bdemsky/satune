#include "autotuner.h"
#include "csolver.h"
#include "searchtuner.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

AutoTuner::AutoTuner(uint _budget) :
	budget(_budget) {
}

void AutoTuner::addProblem(CSolver *solver) {
	solvers.push(solver);
}

long long AutoTuner::evaluate(CSolver * problem, SearchTuner *tuner) {
	CSolver * copy=problem->clone();
	copy->setTuner(tuner);
	int result = copy->startEncoding();
	long long elapsedTime=copy->getElapsedTime();
	long long encodeTime=copy->getEncodeTime();
	long long solveTime=copy->getSolveTime();
	long long metric=elapsedTime;
	delete copy;
	return metric;
}

double AutoTuner::evaluateAll(SearchTuner *tuner) {
	double product=1;
	for(uint i=0;i<solvers.getSize();i++) {
		CSolver * problem=solvers.get(i);
		double score=evaluate(problem, tuner);
		product*=score;
	}
	return pow(product, 1/((double)solvers.getSize()));
}

SearchTuner * AutoTuner::mutateTuner(SearchTuner * oldTuner, uint k) {
	SearchTuner *newTuner=oldTuner->copyUsed();
	uint numSettings=oldTuner->getSize();
	double factor=0.3;//Adjust this factor...
	uint settingsToMutate=(uint)(factor*(((double)numSettings) * k)/(budget));
	if (settingsToMutate < 1)
		settingsToMutate=1;
	model_print("Mutating %u settings\n", settingsToMutate);
	while(settingsToMutate-- != 0) {
		newTuner->randomMutate();
	}
	return newTuner;
}


void AutoTuner::tune() {
	SearchTuner * bestTuner = NULL;
	double bestScore=DBL_MAX;

	SearchTuner * oldTuner=new SearchTuner();
	double base_temperature=evaluateAll(oldTuner);
	double oldScore=base_temperature;

	for (uint i=0;i<budget;i++) {
		SearchTuner *newTuner=mutateTuner(oldTuner, i);
		double newScore=evaluateAll(newTuner);
		double scoreDiff=newScore - oldScore; //smaller is better
		if (newScore < bestScore) {
			bestScore = newScore;
			bestTuner = newTuner->copyUsed();
		}

		double acceptanceP;
		if (scoreDiff < 0) {
			acceptanceP = 1;
		} else {
			double currTemp=base_temperature * (((double)budget - i) / budget);
			acceptanceP = exp(-scoreDiff / currTemp);
		}
		double ran = ((double)random()) / RAND_MAX;
		if (ran <= acceptanceP) {
			oldScore = newScore;
			oldTuner = newTuner;
		} else {
			delete newTuner;
		}
	}
}
