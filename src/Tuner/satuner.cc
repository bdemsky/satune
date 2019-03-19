#include "satuner.h"
#include <float.h>
#include <math.h>
#include "searchtuner.h"
#include <iostream>
#include <fstream>
#include "solver_interface.h"
#include <stdlib.h>

SATuner::SATuner(uint _budget, uint _timeout) :
	BasicTuner(_budget, _timeout)
{
}

SATuner::~SATuner() {

}

void SATuner::rankTunerForProblem(Vector<TunerRecord *> *places, TunerRecord *tuner, Problem *problem, long long metric) {
	uint k = 0;
	for (; k < places->getSize(); k++) {
		if (metric < places->get(k)->getTime(problem))
			break;
	}
	model_print("Problem<%s>:place[%u]=Tuner<%p,%d>\n", problem->getProblem(), k, tuner, tuner->getTunerNumber());
	places->insertAt(k, tuner);
}

void SATuner::removeTunerIndex(Vector<TunerRecord *> *tunerV, int index,  Vector<Vector<TunerRecord *> *> &allplaces) {
	TunerRecord *tuner = tunerV->get(index);
	model_print("Removing Tuner %d\n", tuner->getTunerNumber());
	tunerV->set(index, NULL);
	for (uint i = 0; i < allplaces.getSize(); i++) {
		Vector<TunerRecord *> *places = allplaces.get(i);
		for (uint j = 0; j < places->getSize(); j++) {
			if (tuner == places->get(j)) {
				places->removeAt(j);
				break;
			}
		}
	}

}

void SATuner::removeNullsFromTunerVector( Vector<TunerRecord *> *tunerV) {
	for (int i = tunerV->getSize() - 1; i >= 0; i--) {
		if (tunerV->get(i) == NULL) {
			tunerV->removeAt(i);
		}
	}
	model_print("TunerV size after removing nulls = %u\n", tunerV->getSize());
}

void SATuner::initialize(Vector<TunerRecord *> *tunerV, Vector<Vector<TunerRecord *> *> &allplaces) {
	for (uint ii = 0; ii < problems.getSize(); ii++) {
		allplaces.push(new Vector<TunerRecord *>());
	}
	for (uint j = 0; j < tunerV->getSize(); j++) {
		TunerRecord *tuner = tunerV->get(j);
		for (uint i = 0; i < problems.getSize(); i++) {
			Problem *problem = problems.get(i);
			long long metric = evaluate(problem, tuner);
			ASSERT(tuner->getTime(problem) == -1);
			tuner->addProblem(problem);
			if (metric != -1) {
				tuner->setTime(problem, metric);
			} else {
				tuner->setTime(problem, -2);
			}
			if (metric >= 0) {
				Vector<TunerRecord *> *places = allplaces.get(i);
				rankTunerForProblem(places, tuner, problem, metric);
			}
		}
	}

}

void SATuner::tune() {
	Vector<TunerRecord *> *tunerV = new Vector<TunerRecord *>(&tuners);
	Vector<Vector<TunerRecord *> *> allplaces;
	uint tunerNumber = tuners.getSize();
	//Initialization
	initialize(tunerV, allplaces);
	//Starting the body of algorithm
	for (uint t = budget; t > 0; t--) {
		model_print("Current Temperature = %u\n", t);
		Hashtable<TunerRecord *, int, uint64_t> scores;
		for (uint i = 0; i < tunerNumber; i++) {
			SearchTuner *tmpTuner = mutateTuner(tunerV->get(i)->getTuner(), budget - t);
			int tunerIndex = subTunerIndex(tmpTuner);
			TunerRecord *tmp = NULL;
			if (tunerIndex == -1) {
				tmp = new TunerRecord(tmpTuner);
				tmp->setTunerNumber(allTuners.getSize());
				model_print("Mutated tuner %u to generate tuner %u\n", tunerV->get(i)->getTunerNumber(), tmp->getTunerNumber());
				allTuners.push(tmp);
			} else {
				//Previous tuners might get explored with new combination of tuners.
				tmp = explored.get(tunerIndex);
				model_print("Using exploread tuner <%u>\n", tmp->getTunerNumber());
			}
			tunerV->push(tmp);
		}
		ASSERT(tunerNumber * 2 == tunerV->getSize());
		for (uint j = tunerNumber; j < tunerV->getSize(); j++) {
			TunerRecord *tuner = tunerV->get(j);
			for (uint i = 0; i < problems.getSize(); i++) {
				Problem *problem = problems.get(i);
				long long metric = tuner->getTime(problem);
				if (metric == -1) {
					metric = evaluate(problem, tuner);
					if (tuner->getTime(problem) == -1) {
						tuner->addProblem(problem);
					}
					model_print("%u.Problem<%s>\tTuner<%p, %d>\tMetric<%lld>\n", i, problem->getProblem(),tuner, tuner->getTunerNumber(), metric);
					model_print("*****************************\n");
					if (metric != -1)
						tuner->setTime(problem, metric);
					else
						tuner->setTime(problem, -2);

				}
				if (metric >= 0) {
					ASSERT(i < allplaces.getSize());
					Vector<TunerRecord *> *places = allplaces.get(i);
					model_print("Problem<%s>:place[size=%u]=Tuner<%p,%d>\n", problem->getProblem(), places->getSize(), tuner, tuner->getTunerNumber());
					rankTunerForProblem(places, tuner, problem, metric);
				}
			}
		}
		for (uint ii = 0; ii < problems.getSize(); ii++) {
			Problem *problem = problems.get(ii);
			ASSERT(ii < allplaces.getSize());
			Vector<TunerRecord *> *places = allplaces.get(ii);
			int points = pow(tunerNumber * 1.0, 2 * tunerNumber - 1);
			for (uint k = 0; k < places->getSize() && points; k++) {
				TunerRecord *tuner = places->get(k);
				int currScore = 0;
				if (scores.contains(tuner))
					currScore = scores.get(tuner);
				currScore += points;
				model_print("Problem<%s>\tTuner<%p,%d>\tmetric<%d>\n", problem->getProblem(), tuner, tuner->getTunerNumber(),  currScore);
				model_print("**************************\n");
				scores.put(tuner, currScore);
				points = points / tunerNumber;
			}
		}

		for (uint i = 0; i < tunerNumber; i++) {
			ASSERT(i < tunerV->getSize());
			TunerRecord *tuner1 = tunerV->get(i);
			TunerRecord *tuner2 = tunerV->get(tunerNumber + i);
			ASSERT( tunerNumber + i < tunerV->getSize());
			model_print("Tuner1 = %d \tTuner2 = %d\n", tuner1->getTunerNumber(), tuner2->getTunerNumber());
			ASSERT(scores.contains(tuner1));
			ASSERT(scores.contains(tuner2));
			int score1 = scores.get(tuner1);
			int score2 = scores.get(tuner2);
			if ( score2 > score1 ) {
				removeTunerIndex(tunerV, i, allplaces);
			} else if ( score2 < score1) {
				model_print("score1=%d\tscore2=%d\tt=%u\texp=%f\n", score1, score2, t, exp((score1 - score2) * 1.0 / t));
				double prob = 1 / (exp((score1 - score2) * 1.0 / t) );
				double random = ((double) rand() / (RAND_MAX));
				model_print("prob=%f\trandom=%f\n", prob, random);
				if (prob > random) {
					removeTunerIndex(tunerV, i, allplaces);
				} else {
					removeTunerIndex(tunerV, tunerNumber + i, allplaces);
				}
			} else {
				double random = ((double) rand() / (RAND_MAX));
				int index = random > 0.5 ? i : tunerNumber + i;
				removeTunerIndex(tunerV, index, allplaces);
			}
		}
		removeNullsFromTunerVector(tunerV);

	}
	for (uint ii = 0; ii < allplaces.getSize(); ii++) {
		delete allplaces.get(ii);
	}
	printData();
}
