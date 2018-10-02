#include "autotuner.h"
#include "csolver.h"
#include "searchtuner.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define TIMEOUT 1000s
#define UNSETVALUE -1
#define POSINFINITY 9999999999L

using namespace std::chrono_literals;

int solve(CSolver *solver)
{
	try{
		return solver->solve();
	}
	catch (std::runtime_error &e) {
		return UNSETVALUE;
	}
}

int solveWrapper(CSolver *solver)
{
	std::mutex m;
	std::condition_variable cv;
	int retValue;

	std::thread t([&cv, &retValue, solver]()
		{
								retValue = solve(solver);
								cv.notify_one();
		});

	t.detach();

	{
		std::unique_lock<std::mutex> l(m);
		if (cv.wait_for(l, TIMEOUT) == std::cv_status::timeout)
			throw std::runtime_error("Timeout");
	}

	return retValue;
}


AutoTuner::AutoTuner(uint _budget) :
	budget(_budget), result(UNSETVALUE) {
}

void AutoTuner::addProblem(CSolver *solver) {
	solvers.push(solver);
}

long long AutoTuner::evaluate(CSolver *problem, SearchTuner *tuner) {
	CSolver *copy = problem->clone();
	copy->setTuner(tuner);
	model_print("**********************\n");
	long long metric = 0L;
	try {
		int sat = solveWrapper(copy);
		if (result == UNSETVALUE)
			result = sat;
		else if (result != sat) {
			model_print("&&&&&&&&&&&&&&&&&& Result has changed &&&&&&&&&&&&&\n");
			copy->printConstraints();
		}
		metric = copy->getElapsedTime();
	}
	catch (std::runtime_error &e) {
		metric = POSINFINITY;
		model_print("TimeOut has hit\n");
	}

	delete copy;
	return metric;
}

double AutoTuner::evaluateAll(SearchTuner *tuner) {
	double product = 1;
	for (uint i = 0; i < solvers.getSize(); i++) {
		CSolver *problem = solvers.get(i);
		double score = evaluate(problem, tuner);
		product *= score;
	}
	return pow(product, 1 / ((double)solvers.getSize()));
}

SearchTuner *AutoTuner::mutateTuner(SearchTuner *oldTuner, uint k) {
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


void AutoTuner::tune() {
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
	bestTuner->serialize();
	model_print("Received score %f\n", bestScore);
	if (bestTuner != NULL)
		delete bestTuner;
	delete oldTuner;
}
