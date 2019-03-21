/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   kmeanstuner.cc
 * Author: hamed
 *
 * Created on December 19, 2018, 4:16 PM
 */

#include "kmeanstuner.h"
#include "float.h"
#include "searchtuner.h"
#include "math.h"

KMeansTuner::KMeansTuner(uint budget, uint _rounds, uint timeout) :
	BasicTuner(budget, timeout),
	rounds(_rounds) {
}

KMeansTuner::~KMeansTuner() {
}

void clearVector(Vector<TunerRecord *> *tunerV) {
	for (uint j = 0; j < tunerV->getSize(); j++) {
		TunerRecord *tuner = tunerV->get(j);
		tuner->problems.clear();
	}
}

void KMeansTuner::tune() {
	Vector<TunerRecord *> *tunerV = new Vector<TunerRecord *>(&tuners);
	for (uint i = 0; i < rounds; i++) {
		clearVector(tunerV);
		mapProblemsToTuners(tunerV);
		improveTuners(tunerV);
	}
	model_print("Best tuners\n");
	for (uint j = 0; j < tunerV->getSize(); j++) {
		TunerRecord *tuner = tunerV->get(j);
		char buffer[256];
		sprintf(buffer, "tuner%u.conf", j);
		tuner->getTuner()->serialize(buffer);
		tuner->getTuner()->print();
	}
	delete tunerV;
}

void KMeansTuner::improveTuners(Vector<TunerRecord *> *tunerV) {
	for (uint j = 0; j < tunerV->getSize(); j++) {
		TunerRecord *tuner = tunerV->get(j);
		TunerRecord *newtuner = tune(tuner);
		tunerV->set(j, newtuner);
	}
}

double KMeansTuner::evaluateAll(TunerRecord *tuner) {
	double product = 1;
	for (uint i = 0; i < tuner->problemsSize(); i++) {
		Problem *problem = tuner->getProblem(i);
		long long metric = tuner->getTime(problem);
		if (metric == -1) {
			metric = evaluate(problem, tuner);
			if (metric != -1)
				tuner->setTime(problem, metric);
			else
				tuner->setTime(problem, -2);
		}

		double score = metric;
		if (metric < 0)
			score = timeout;
		product *= score;
	}
	return pow(product, 1 / ((double)tuner->problemsSize()));
}

TunerRecord *KMeansTuner::tune(TunerRecord *tuner) {
	TunerRecord *bestTuner = NULL;
	double bestScore = DBL_MAX;

	TunerRecord *oldTuner = tuner;
	double base_temperature = evaluateAll(oldTuner);
	double oldScore = base_temperature;

	for (uint i = 0; i < budget; i++) {
		SearchTuner *tmpTuner = mutateTuner(oldTuner->getTuner(), i);
		TunerRecord *newTuner = oldTuner->changeTuner(tmpTuner);
		newTuner->setTunerNumber( allTuners.getSize() );
		allTuners.push(newTuner);
		double newScore = evaluateAll(newTuner);
		newTuner->getTuner()->printUsed();
		model_print("Received score %f\n", newScore);
		double scoreDiff = newScore - oldScore;	//smaller is better
		if (newScore < bestScore) {
			bestScore = newScore;
			bestTuner = newTuner;
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
			oldScore = newScore;
			oldTuner = newTuner;
		}
	}

	return bestTuner;
}

void KMeansTuner::mapProblemsToTuners(Vector<TunerRecord *> *tunerV) {
	for (uint i = 0; i < problems.getSize(); i++) {
		Problem *problem = problems.get(i);
		TunerRecord *bestTuner = NULL;
		long long bestscore = 0;
		for (uint j = 0; j < tunerV->getSize(); j++) {
			TunerRecord *tuner = tunerV->get(j);
			long long metric = tuner->getTime(problem);
			if (metric == -1) {
				metric = evaluate(problem, tuner);
				if (metric != -1)
					tuner->setTime(problem, metric);
				else
					tuner->setTime(problem, -2);
			}
			if ((bestTuner == NULL && metric >= 0) ||
					(metric < bestscore && metric >= 0)) {
				bestTuner = tuner;
				bestscore = metric;
			}
		}
		if (bestTuner != NULL)
			bestTuner->addProblem(problem);
	}
}