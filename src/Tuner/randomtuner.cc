#include "randomtuner.h"
#include "csolver.h"
#include "searchtuner.h"
#include "multituner.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <limits>

#define UNSETVALUE -1

RandomTuner::RandomTuner(uint _budget, uint _timeout) :
	budget(_budget), timeout(_timeout), execnum(0) {
}

RandomTuner::~RandomTuner() {
	for (uint i = 0; i < problems.getSize(); i++)
		ourfree(problems.get(i));
	for (uint i = 0; i < allTuners.getSize(); i++)
		delete allTuners.get(i);
}

void RandomTuner::addProblem(const char *filename) {
	Problem *p = new Problem(filename);
	p->problemnumber = problems.getSize();
	problems.push(p);
}

void RandomTuner::printData() {
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

bool RandomTuner::tunerExists(SearchTuner *tuner){
	for(uint i=0; i< explored.getSize(); i++){
		if(explored.get(i)->getTuner()->equalUsed(tuner))
			return true;
	}
	return false;
}

void RandomTuner::addTuner(SearchTuner *tuner) {
	TunerRecord *t = new TunerRecord(tuner);
	tuners.push(t);
	t->tunernumber = allTuners.getSize();
	allTuners.push(t);
}

long long RandomTuner::evaluate(Problem *problem, TunerRecord *tuner) {
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

	//compute timeout
	uint timeinsecs = problem->besttime / NANOSEC;
	uint adaptive = (timeinsecs > 30) ? timeinsecs * 5 : 150;
	uint maxtime = (adaptive < timeout) ? adaptive : timeout;

	//Do run
	snprintf(buffer, sizeof(buffer), "./run.sh deserializerun %s %u tuner%u result%u > log%u", problem->getProblem(), maxtime, execnum, execnum, execnum);
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
		updateTimeout(problem, metric);
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
	if (sat == IS_INDETER && metric != -1) {//The case when we have a timeout
		metric = -1;
	}
	return metric;
}

void RandomTuner::updateTimeout(Problem *problem, long long metric) {
	if (metric < problem->besttime) {
		problem->besttime = metric;
	}
}

void RandomTuner::tune() {
	for (uint r = 0; r < budget; r++) {
		model_print("Round %u of %u\n", r, budget);
		for (uint i = 0; i < tuners.getSize(); i++){
			TunerRecord *tuner = tuners.get(i);
			bool isNew = true;
			for (uint j = 0; j < problems.getSize(); j++){
				Problem *problem = problems.get(j);
				long long metric = tuner->getTime(problem);
				if(metric == -1){
					metric = evaluate(problem, tuner);
					ASSERT(tuner->getTime(problem) == -1);
					tuner->problems.push(problem);
					model_print("%u.Problem<%s>\tTuner<%p, %d>\tMetric<%lld>\n", i, problem->problem,tuner, tuner->tunernumber, metric);
					if (metric != -1)
						tuner->setTime(problem, metric);
					else
						tuner->setTime(problem, -2);
					if(tunerExists(tuner->getTuner())){
						//Solving the first problem and noticing the tuner
						//already exists
						isNew = false;
						break;
					}
				}
			}
			if(isNew){
				explored.push(tuner);
			}
			
		}
		uint tSize = tuners.getSize();
		for (uint i = 0; i < tSize; i++) {
			SearchTuner *tmpTuner = mutateTuner(tuners.get(i)->getTuner(), budget);
			while(subtuneIndex(tmpTuner) != -1){
				tmpTuner->randomMutate();
			}
			TunerRecord *tmp = new TunerRecord(tmpTuner);
			tmp->tunernumber = allTuners.getSize();
			model_print("Mutated tuner %u to generate tuner %u\n", tuners.get(i)->tunernumber, tmp->tunernumber);
			allTuners.push(tmp);
			tuners.set(i, tmp);
		}
	}
	printData();
	
}

SearchTuner *RandomTuner::mutateTuner(SearchTuner *oldTuner, uint k) {
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

int RandomTuner::subtuneIndex(SearchTuner *newTuner){
	for (uint i=0; i< explored.getSize(); i++){
		SearchTuner *tuner = explored.get(i)->getTuner();
		if(tuner->isSubTunerof(newTuner)){
			return i;
		}
	}
	return -1;
}
