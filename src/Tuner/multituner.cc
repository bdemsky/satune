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

Problem::Problem(const char *_problem) : problemnumber(-1), result(UNSETVALUE) {
	uint len = strlen(_problem);
	problem = (char *) ourmalloc(len + 1);
	memcpy(problem, _problem, len + 1);
}

Problem::~Problem() {
	ourfree(problem);
}

void TunerRecord::setTime(Problem *problem, long long time) {
	timetaken.put(problem, time);
}

long long TunerRecord::getTime(Problem *problem) {
	if (timetaken.contains(problem))
		return timetaken.get(problem);
	else return -1;
}

TunerRecord *TunerRecord::changeTuner(SearchTuner *_newtuner) {
	TunerRecord *tr = new TunerRecord(_newtuner);
	for (uint i = 0; i < problems.getSize(); i++) {
		tr->problems.push(problems.get(i));
	}
	return tr;
}

MultiTuner::MultiTuner(uint _budget, uint _rounds, uint _timeout) :
	budget(_budget), rounds(_rounds), timeout(_timeout), execnum(0) {
}

MultiTuner::~MultiTuner() {
	for (uint i = 0; i < problems.getSize(); i++)
		ourfree(problems.get(i));
	for (uint i = 0; i < allTuners.getSize(); i++)
		delete allTuners.get(i);
}

void MultiTuner::addProblem(const char *filename) {
	Problem *p = new Problem(filename);
	p->problemnumber = problems.getSize();
	problems.push(p);
}

void MultiTuner::printData() {
	model_print("*********** DATA DUMP ***********\n");
	for (uint i = 0; i < allTuners.getSize(); i++) {
		TunerRecord *tuner = allTuners.get(i);
		SearchTuner *stun = tuner->getTuner();
		model_print("Tuner %u\n", i);
		stun->print();
		model_print("----------------------------------\n\n\n");
		for (uint j = 0; j < tuner->problems.getSize(); j++) {
			Problem *problem = tuner->problems.get(j);
			model_print("Problem %s\n", problem->getProblem());
			model_print("Time = %lld\n", tuner->getTime(problem));
		}
	}
}

void MultiTuner::addTuner(SearchTuner *tuner) {
	TunerRecord *t = new TunerRecord(tuner);
	tuners.push(t);
	t->tunernumber = allTuners.getSize();
	allTuners.push(t);
}


void MultiTuner::readData(uint numRuns) {
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
			allTuners.set(tunernumber, new TunerRecord(new SearchTuner(buffer)));
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
		if (problem->result == UNSETVALUE && sat != IS_INDETER) {
			problem->result = sat;
		} else if (problem->result != sat && sat != IS_INDETER) {
			model_print("******** Result has changed ********\n");
		}

		if (metric != -1) {
			if (tuner->getTime(problem) == -1)
				tuner->problems.push(problem);
			tuner->setTime(problem, metric);
		}

	}

}

long long MultiTuner::evaluate(Problem *problem, TunerRecord *tuner) {
	char buffer[512];
	{
		snprintf(buffer, sizeof(buffer), "problem%u", execnum);

		ofstream myfile;
		myfile.open (buffer, ios::out);


		if (myfile.is_open()) {
			myfile << problem->getProblem() << endl;
			myfile << problem->problemnumber << endl;
			myfile.close();
		}
	}

	{
		snprintf(buffer, sizeof(buffer), "tunernum%u", execnum);

		ofstream myfile;
		myfile.open (buffer, ios::out);


		if (myfile.is_open()) {
			myfile << tuner->tunernumber << endl;
			myfile.close();
		}
	}

	//Write out the tuner
	snprintf(buffer, sizeof(buffer), "tuner%u", execnum);
	tuner->getTuner()->serialize(buffer);

	//Do run
	snprintf(buffer, sizeof(buffer), "./run.sh deserializerun %s %u tuner%u result%u > log%u", problem->getProblem(), timeout, execnum, execnum, execnum);
	int status = system(buffer);

	long long metric = -1;
	int sat = IS_INDETER;

	if (status == 0) {
		//Read data in from results file
		snprintf(buffer, sizeof(buffer), "result%u", execnum);

		ifstream myfile;
		myfile.open (buffer, ios::in);


		if (myfile.is_open()) {
			myfile >> metric;
			myfile >> sat;
			myfile.close();
		}

		snprintf(buffer, sizeof(buffer), "tuner%uused", execnum);
		tuner->getTuner()->addUsed(buffer);
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

void MultiTuner::tuneComp() {
	Vector<TunerRecord *> *tunerV = new Vector<TunerRecord *>(&tuners);
	for (uint b = 0; b < budget; b++) {
		model_print("Round %u of %u\n", b, budget);
		uint tSize = tunerV->getSize();
		for (uint i = 0; i < tSize; i++) {
			SearchTuner *tmpTuner = mutateTuner(tunerV->get(i)->getTuner(), b);
			TunerRecord *tmp = new TunerRecord(tmpTuner);
			tmp->tunernumber = allTuners.getSize();
			model_print("Mutated tuner %u to generate tuner %u\n", tunerV->get(i)->tunernumber, tmp->tunernumber);
			allTuners.push(tmp);
			tunerV->push(tmp);
		}

		Hashtable<TunerRecord *, int, uint64_t> scores;
		for (uint i = 0; i < problems.getSize(); i++) {
			Problem *problem = problems.get(i);
			Vector<TunerRecord *> places;
			for (uint j = 0; j < tunerV->getSize(); j++) {
				TunerRecord *tuner = tunerV->get(j);
				long long metric = tuner->getTime(problem);
				if (metric == -1) {
					metric = evaluate(problem, tuner);
					if (tuner->getTime(problem) == -1) {
						tuner->problems.push(problem);
					}
					DEBUG("%u.Problem<%s>\tTuner<%p>\tMetric<%lld>\n", i, problem->problem,tuner, metric);
					DEBUG("*****************************\n");
					if (metric != -1)
						tuner->setTime(problem, metric);
				}
				if (metric != -1) {
					uint k = 0;
					for (; k < places.getSize(); k++) {
						if (metric < places.get(k)->getTime(problem))
							break;
					}
					DEBUG("place[%u]=Tuner<%p>\n", k, tuner);
					places.insertAt(k, tuner);
				}
			}
			int points = 9;
			for (uint k = 0; k < places.getSize() && points; k++) {
				TunerRecord *tuner = places.get(k);
				int currScore = 0;
				if (scores.contains(tuner))
					currScore = scores.get(tuner);
				currScore += points;
				DEBUG("Problem<%s>\tTuner<%p>\tmetric<%d>\n", problem->problem, tuner, currScore);
				DEBUG("**************************\n");
				scores.put(tuner, currScore);
				points = points / 3;
			}
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
				if (score < tscore)
					break;
			}
			DEBUG("ranking[%u]=tuner<%p>(Score=%d)\n", j, tuner, score);
			DEBUG("************************\n");
			ranking.insertAt(j, tuner);
		}
		DEBUG("tunerSize=%u\trankingSize=%u\ttunerVSize=%u\n", tuners.getSize(), ranking.getSize(), tunerV->getSize());
		for (uint i = tuners.getSize(); i < ranking.getSize(); i++) {
			TunerRecord *tuner = ranking.get(i);
			model_print("Removing tuner %u\n", tuner->tunernumber);
			for (uint j = 0; j < tunerV->getSize(); j++) {
				if (tunerV->get(j) == tuner)
					tunerV->removeAt(j);
			}
		}
	}
	printData();
}

void MultiTuner::mapProblemsToTuners(Vector<TunerRecord *> *tunerV) {
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
			}
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

void clearVector(Vector<TunerRecord *> *tunerV) {
	for (uint j = 0; j < tunerV->getSize(); j++) {
		TunerRecord *tuner = tunerV->get(j);
		tuner->problems.clear();
	}
}

void MultiTuner::tuneK() {
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

void MultiTuner::improveTuners(Vector<TunerRecord *> *tunerV) {
	for (uint j = 0; j < tunerV->getSize(); j++) {
		TunerRecord *tuner = tunerV->get(j);
		TunerRecord *newtuner = tune(tuner);
		tunerV->set(j, newtuner);
	}
}

double MultiTuner::evaluateAll(TunerRecord *tuner) {
	double product = 1;
	for (uint i = 0; i < tuner->problems.getSize(); i++) {
		Problem *problem = tuner->problems.get(i);
		long long metric = tuner->getTime(problem);
		if (metric == -1) {
			metric = evaluate(problem, tuner);
			if (metric != -1)
				tuner->setTime(problem, metric);
		}

		double score = metric;
		product *= score;
	}
	return pow(product, 1 / ((double)tuner->problems.getSize()));
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

TunerRecord *MultiTuner::tune(TunerRecord *tuner) {
	TunerRecord *bestTuner = NULL;
	double bestScore = DBL_MAX;

	TunerRecord *oldTuner = tuner;
	double base_temperature = evaluateAll(oldTuner);
	double oldScore = base_temperature;

	for (uint i = 0; i < budget; i++) {
		SearchTuner *tmpTuner = mutateTuner(oldTuner->getTuner(), i);
		TunerRecord *newTuner = oldTuner->changeTuner(tmpTuner);
		newTuner->tunernumber = allTuners.getSize();
		allTuners.push(newTuner);
		double newScore = evaluateAll(newTuner);
		newTuner->tuner->printUsed();
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

