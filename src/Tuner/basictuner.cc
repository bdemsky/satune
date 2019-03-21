/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   basictuner.cc
 * Author: hamed
 * 
 * Created on December 17, 2018, 2:02 PM
 */

#include "basictuner.h"
#include "common.h"
#include "csolver.h"
#include "searchtuner.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

Problem::Problem(const char *_problem) :
	problemnumber(-1),
	result(TUNERUNSETVALUE),
	besttime(LLONG_MAX)
{
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

void TunerRecord::print(){
	model_print("*************TUNER NUMBER=%d***********\n", tunernumber);
	tuner->print();
	model_print("&&&&&&&&&&&&&USED SETTINGS &&&&&&&&&&&&\n");
	tuner->printUsed();
	model_print("\n");
}

void TunerRecord::printProblemsInfo(){
	for (uint j = 0; j < problems.getSize(); j++) {
		Problem *problem = problems.get(j);
		model_print("Problem %s\n", problem->getProblem());
		model_print("Time = %lld\n", getTime(problem));
	}
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


BasicTuner::BasicTuner(uint _budget, uint _timeout) :
	budget(_budget), timeout(_timeout), execnum(0){
}

BasicTuner::~BasicTuner() {
	for (uint i = 0; i < problems.getSize(); i++)
		ourfree(problems.get(i));
	for (uint i = 0; i < allTuners.getSize(); i++)
		delete allTuners.get(i);
}

void BasicTuner::addProblem(const char *filename) {
	Problem *p = new Problem(filename);
	p->setProblemNumber(problems.getSize());
	problems.push(p);
}

void BasicTuner::printData() {
	model_print("*********** DATA DUMP ***********\n");
	for (uint i = 0; i < allTuners.getSize(); i++) {
		TunerRecord *tuner = allTuners.get(i);
		SearchTuner *stun = tuner->getTuner();
		model_print("Tuner %u\n", i);
		stun->print();
		model_print("----------------------------------\n\n\n");
		for (uint j = 0; j < tuner->problemsSize(); j++) {
			Problem *problem = tuner->getProblem(j);
			model_print("Problem %s\n", problem->getProblem());
			model_print("Time = %lld\n", tuner->getTime(problem));
		}
	}
}

bool BasicTuner::tunerExists(TunerRecord *tunerec){
	SearchTuner *tuner = tunerec->getTuner();
	for(uint i=0; i< explored.getSize(); i++){
		if(explored.get(i)->getTuner()->equalUsed(tuner)){
			model_print("************Tuner <%d> is replicate of Tuner <%d>\n", tunerec->getTunerNumber(), explored.get(i)->getTunerNumber());
			return true;
		}
	}
	return false;
}

void BasicTuner::addTuner(SearchTuner *tuner) {
	TunerRecord *t = new TunerRecord(tuner);
	tuners.push(t);
	t->setTunerNumber( allTuners.getSize() );
	allTuners.push(t);
}

long long BasicTuner::evaluate(Problem *problem, TunerRecord *tuner) {
	char buffer[512];
	{
		snprintf(buffer, sizeof(buffer), "problem%u", execnum);

		ofstream myfile;
		myfile.open (buffer, ios::out);


		if (myfile.is_open()) {
			myfile << problem->getProblem() << endl;
			myfile << problem->getProblemNumber() << endl;
			myfile.close();
		}
	}

	{
		snprintf(buffer, sizeof(buffer), "tunernum%u", execnum);

		ofstream myfile;
		myfile.open (buffer, ios::out);


		if (myfile.is_open()) {
			myfile << tuner->getTunerNumber() << endl;
			myfile.close();
		}
	}

	//Write out the tuner
	snprintf(buffer, sizeof(buffer), "tuner%u", execnum);
	tuner->getTuner()->serialize(buffer);

	//compute timeout
	uint timeinsecs = problem->getBestTime() / NANOSEC;
	uint adaptive = (timeinsecs > 30) ? timeinsecs * 5 : 150;
	uint maxtime = (adaptive < timeout) ? adaptive : timeout;

	//Do run
	snprintf(buffer, sizeof(buffer), "./run.sh deserializerun %s %u tuner%u result%u > log%u", problem->getProblem(), maxtime, execnum, execnum, execnum);
	model_print("Running: %s\n", buffer);
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

	if (problem->getResult() == TUNERUNSETVALUE && sat != IS_INDETER) {
		problem->setResult( sat );
	} else if (problem->getResult() != sat && sat != IS_INDETER) {
		model_print("******** Result has changed ********\n");
	}
	if (sat == IS_INDETER && metric != -1) {//The case when we have a timeout
		metric = -1;
	}
	return metric;
}

SearchTuner *BasicTuner::mutateTuner(SearchTuner *oldTuner, uint k) {
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

int BasicTuner::subTunerIndex(SearchTuner *newTuner){
	for (uint i=0; i< explored.getSize(); i++){
		SearchTuner *tuner = explored.get(i)->getTuner();
		if(tuner->isSubTunerof(newTuner)){
			return i;
		}
	}
	return -1;
}


void BasicTuner::updateTimeout(Problem *problem, long long metric) {
	if (metric < problem->getBestTime()) {
		problem->setBestTime( metric );
	}
}
