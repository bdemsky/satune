#include "multituner.h"
#include "csolver.h"
#include "searchtuner.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <iostream>
#include <fstream>


#define UNSETVALUE -1
#define TIMEOUTSEC 5000

Problem::Problem(const char *_problem) : result(UNSETVALUE) {
	uint len = strlen(_problem);
	problem = (char *) ourmalloc(len + 1);
	memcpy(problem, _problem, len + 1);
}

Problem::~Problem() {
	ourfree(problem);
}

MultiTuner::MultiTuner(uint _budget, uint _timeout) :
	budget(_budget), timeout(_timeout), execnum(0) {
}

MultiTuner::~MultiTuner() {
	for (uint i = 0; i < problems.getSize(); i++)
		ourfree(problems.get(i));
}

void MultiTuner::addProblem(const char *filename) {
	problems.push(new Problem(filename));
}

void MultiTuner::addTuner(SearchTuner *tuner) {
	tuners.push(new TunerRecord(tuner));
}

long long MultiTuner::evaluate(Problem *problem, SearchTuner *tuner) {
	char buffer[512];
	//Write out the tuner
	snprintf(buffer, sizeof(buffer), "tuner%u", execnum);
	tuner->serialize(buffer);

	//Do run
	snprintf(buffer, sizeof(buffer), "deserializerun %s %u tuner%u result%s%u > log%u", problem->getProblem(), timeout, execnum, problem->getProblem(), execnum, execnum);
	int status = system(buffer);

	long long metric = -1;
	int sat = IS_INDETER;
	if (status == 0) {
		//Read data in from results file
		snprintf(buffer, sizeof(buffer), "result%s%u", problem->getProblem(), execnum);

		ifstream myfile;
		myfile.open (buffer, ios::in);


		if (myfile.is_open()) {
			myfile >> metric;
			myfile >> sat;
			myfile.close();
		}
	}
	//Increment execution count
	execnum++;

	if (problem->result == UNSETVALUE && sat != IS_INDETER) {
		problem->result = sat;
	} else if (problem->result != sat && sat != IS_INDETER) {
		model_print("******** Result has changed ********\n");
	}
	return metric;
}

void MultiTuner::mapProblemsToTuners(Vector<TunerRecord *> *tunerV) {
	for (uint i = 0; i < problems.getSize(); i++) {
		Problem *problem = problems.get(i);
		TunerRecord *bestTuner = NULL;
		long long bestscore = 0;
		for (uint j = 0; j < tunerV->getSize(); j++) {
			TunerRecord *tuner = tunerV->get(j);
			long long metric = evaluate(problem, tuner->getTuner());
			if ((bestTuner == NULL && metric != -1) ||
					(metric < bestscore && metric != -1)) {
				bestTuner = tuner;
				bestscore = metric;
			}
		}
		if (bestTuner != NULL)
			bestTuner->problems.push(problem);
	}
}


void MultiTuner::tuneK() {
	Vector<TunerRecord *> *tunerV = new Vector<TunerRecord *>(&tuners);


	delete tunerV;
}

double MultiTuner::evaluateAll(SearchTuner *tuner) {
	double product = 1;
	for (uint i = 0; i < problems.getSize(); i++) {
		Problem *problem = problems.get(i);
		double score = evaluate(problem, tuner);
		product *= score;
	}
	return pow(product, 1 / ((double)problems.getSize()));
}

SearchTuner *MultiTuner::mutateTuner(SearchTuner *oldTuner, uint k) {
	SearchTuner *newTuner = oldTuner->copyUsed();
	uint numSettings = oldTuner->getSize();
	uint settingsToMutate = (uint)(AUTOTUNERFACTOR * (((double)numSettings) * (budget - k)) / (budget));
	if (settingsToMutate < 1)
		settingsToMutate = 1;
	model_print("Mutating %u settings\n", settingsToMutate);
	while (settingsToMutate-- != 0) {
		newTuner->randomMutate();
	}
	return newTuner;
}

void MultiTuner::tune() {
	SearchTuner *bestTuner = NULL;
	double bestScore = DBL_MAX;

	SearchTuner *oldTuner = new SearchTuner();
	double base_temperature = evaluateAll(oldTuner);
	double oldScore = base_temperature;

	for (uint i = 0; i < budget; i++) {
		SearchTuner *newTuner = mutateTuner(oldTuner, i);
		double newScore = evaluateAll(newTuner);
		newTuner->printUsed();
		model_print("Received score %f\n", newScore);
		double scoreDiff = newScore - oldScore;	//smaller is better
		if (newScore < bestScore) {
			if (bestTuner != NULL)
				delete bestTuner;
			bestScore = newScore;
			bestTuner = newTuner->copyUsed();
		}

		double acceptanceP;
		if (scoreDiff < 0) {
			acceptanceP = 1;
		} else {
			double currTemp = base_temperature * (((double)budget - i) / budget);
			acceptanceP = exp(-scoreDiff / currTemp);
		}
		double ran = ((double)random()) / RAND_MAX;
		if (ran <= acceptanceP) {
			delete oldTuner;
			oldScore = newScore;
			oldTuner = newTuner;
		} else {
			delete newTuner;
		}
	}
	model_print("Best tuner:\n");
	bestTuner->print();
	model_print("Received score %f\n", bestScore);
	if (bestTuner != NULL)
		delete bestTuner;
	delete oldTuner;
}
