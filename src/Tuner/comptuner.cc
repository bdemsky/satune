#include "comptuner.h"
#include <float.h>
#include <math.h>
#include "searchtuner.h"
#include <iostream>
#include <fstream>
#include <limits>
#include "solver_interface.h"

CompTuner::CompTuner(uint _budget, uint _timeout) :
	BasicTuner(_budget, _timeout)
{
}

CompTuner::~CompTuner() {

}

void CompTuner::findBestTwoTuners() {
	if (allTuners.getSize() < 2) {
		printData();
		return;
	}
	TunerRecord *bestTuners[2];
	double score = DBL_MAX;
	for (uint j = 0; j < allTuners.getSize() - 1; j++) {
		for (uint k = j + 1; k < allTuners.getSize(); k++) {
			TunerRecord *tuner1 = allTuners.get(j);
			TunerRecord *tuner2 = allTuners.get(k);
			double mintimes[problems.getSize()];
			for (uint l = 0; l < problems.getSize(); l++) {
				Problem *problem = problems.get(l);
				long long time1 = tuner1->getTime(problem);
				if(time1 < 0){
					time1=LLONG_MAX;
				}
				long long time2 = tuner2->getTime(problem);
				if(time2 < 0){
					time2 = LLONG_MAX;
				}
				mintimes[l] = pow(min(time1,time2), (double)1 / problems.getSize());
			}
			double result = 1;
			for (uint l = 0; l < problems.getSize(); l++) {
				result *= mintimes[l];
			}
			if (result < score) {
				score = result;
				bestTuners[0] = tuner1;
				bestTuners[1] = tuner2;
			}
		}
	}
	model_print("Best 2 tuners:\n");
	for (uint i = 0; i < 2; i++) {
		TunerRecord *tuner = bestTuners[i];
		SearchTuner *stun = tuner->getTuner();
		char buffer[512];
		snprintf(buffer, sizeof(buffer), "best%u.tuner", i);
		stun->serialize(buffer);
		model_print("Tuner %u\n", tuner->getTunerNumber());
		stun->print();
		tuner->printProblemsInfo();
		model_print("----------------------------------\n\n\n");
	}
}


void CompTuner::readData(uint numRuns) {
	for (uint i = 0; i < numRuns; i++) {
		ifstream myfile;
		char buffer[512];
		uint tunernumber;
		snprintf(buffer, sizeof(buffer), "tunernum%u", i);
		myfile.open (buffer, ios::in);
		myfile >> tunernumber;
		myfile.close();
		if (allTuners.getSize() <= tunernumber)
			allTuners.setSize(tunernumber + 1);
		if (allTuners.get(tunernumber) == NULL) {
			snprintf(buffer, sizeof(buffer), "tuner%u", i);
			allTuners.set(tunernumber, new TunerRecord(new SearchTuner(buffer), tunernumber));
		}
		//Add any new used records
		snprintf(buffer, sizeof(buffer), "tuner%uused", i);
		TunerRecord *tuner = allTuners.get(tunernumber);
		tuner->getTuner()->addUsed(buffer);

		char problemname[512];
		uint problemnumber;
		snprintf(buffer, sizeof(buffer), "problem%u", i);
		myfile.open(buffer, ios::in);
		myfile.getline(problemname, sizeof(problemname));
		myfile >> problemnumber;
		myfile.close();
		if (problems.getSize() <= problemnumber)
			problems.setSize(problemnumber + 1);
		if (problems.get(problemnumber) == NULL)
			problems.set(problemnumber, new Problem(problemname));
		Problem *problem = problems.get(problemnumber);
		long long metric = -1;
		int sat = IS_INDETER;
		//Read data in from results file
		snprintf(buffer, sizeof(buffer), "result%u", i);

		myfile.open (buffer, ios::in);


		if (myfile.is_open()) {
			myfile >> metric;
			myfile >> sat;
			myfile.close();
		}
		if (problem->getResult() == TUNERUNSETVALUE && sat != IS_INDETER) {
			problem->setResult(sat);
		} else if (problem->getResult() != sat && sat != IS_INDETER) {
			model_print("******** Result has changed ********\n");
		}

		if (metric != -1) {
			if (tuner->getTime(problem) == -1)
				tuner->addProblem(problem);
			tuner->setTime(problem, metric);
		}

	}

}

void CompTuner::tune() {
	Vector<TunerRecord *> *tunerV = new Vector<TunerRecord *>(&tuners);
	for (uint b = 0; b < budget; b++) {
		model_print("Round %u of %u\n", b, budget);
		Hashtable<TunerRecord *, int, uint64_t> scores;
		Vector<Vector<TunerRecord *> *> allplaces;
		for (uint ii = 0; ii < problems.getSize(); ii++) {
			allplaces.push(new Vector<TunerRecord *>());
		}
		for (uint j = 0; j < tunerV->getSize(); j++) {
			TunerRecord *tuner = tunerV->get(j);

			for (uint i = 0; i < problems.getSize(); i++) {
				Vector<TunerRecord *> *places = allplaces.get(i);
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

					if (tunerExists(tuner)) {
						//Solving the problem and noticing the tuner
						//already exists
						tuner->setDuplicate(true);
						break;
					}
				}
				if (metric >= 0) {
					uint k = 0;
					for (; k < places->getSize(); k++) {
						if (metric < places->get(k)->getTime(problem))
							break;
					}
					model_print("place[%u]=Tuner<%p,%d>\n", k, tuner, tuner->getTunerNumber());
					places->insertAt(k, tuner);
				}
			}
		}
		for (uint ii = 0; ii < problems.getSize(); ii++) {
			Problem *problem = problems.get(ii);
			Vector<TunerRecord *> *places = allplaces.get(ii);
			int points = 9;
			for (uint k = 0; k < places->getSize() && points; k++) {
				TunerRecord *tuner = places->get(k);
				if (tuner->isDuplicate()) {
					continue;
				}
				int currScore = 0;
				if (scores.contains(tuner))
					currScore = scores.get(tuner);
				currScore += points;
				model_print("Problem<%s>\tTuner<%p,%d>\tmetric<%d>\n", problem->getProblem(), tuner, tuner->getTunerNumber(),  currScore);
				model_print("**************************\n");
				scores.put(tuner, currScore);
				points = points / 3;
			}
		}
		for (uint ii = 0; ii < problems.getSize(); ii++) {
			delete allplaces.get(ii);
		}
		Vector<TunerRecord *> ranking;
		for (uint i = 0; i < tunerV->getSize(); i++) {
			TunerRecord *tuner = tunerV->get(i);
			int score = 0;
			if (scores.contains(tuner))
				score = scores.get(tuner);
			uint j = 0;
			for (; j < ranking.getSize(); j++) {
				TunerRecord *t = ranking.get(j);
				int tscore = 0;
				if (scores.contains(t))
					tscore = scores.get(t);
				if (score > tscore)
					break;
			}
			model_print("ranking[%u]=tuner<%p,%u>(Score=%d)\n", j, tuner, tuner->getTunerNumber(), score);
			model_print("************************\n");
			ranking.insertAt(j, tuner);
		}
		model_print("tunerSize=%u\trankingSize=%u\ttunerVSize=%u\n", tuners.getSize(), ranking.getSize(), tunerV->getSize());
		for (uint i = tuners.getSize(); i < ranking.getSize(); i++) {
			TunerRecord *tuner = ranking.get(i);
			model_print("Removing tuner %u\n", tuner->getTunerNumber());
			for (uint j = 0; j < tunerV->getSize(); j++) {
				if (tunerV->get(j) == tuner)
					tunerV->removeAt(j);
			}
		}
		uint tSize = tunerV->getSize();
		for (uint i = 0; i < tSize; i++) {
			SearchTuner *tmpTuner = mutateTuner(tunerV->get(i)->getTuner(), b);
			while (subTunerIndex(tmpTuner) != -1) {
				model_print("******** New Tuner already explored...\n");
				delete tmpTuner;
				tmpTuner = mutateTuner(tunerV->get(i)->getTuner(), b);
			}
			TunerRecord *tmp = new TunerRecord(tmpTuner);
			tmp->setTunerNumber(allTuners.getSize());
			model_print("Mutated tuner %u to generate tuner %u\n", tunerV->get(i)->getTunerNumber(), tmp->getTunerNumber());
			allTuners.push(tmp);
			tunerV->push(tmp);
		}
	}
	printData();
}
