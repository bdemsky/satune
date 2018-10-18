#ifndef MULTITUNER_H
#define MULTITUNER_H
#include "classlist.h"
#include "structs.h"

#define AUTOTUNERFACTOR 0.3

class Problem {
public:
	Problem(const char *problem);
	char *getProblem() {return problem;}
	~Problem();
	CMEMALLOC;
private:
	int problemnumber;
	int result;
	char *problem;
        double besttime;
	friend class MultiTuner;
};

class TunerRecord {
public:
	TunerRecord(SearchTuner *_tuner) : tuner(_tuner), tunernumber(-1) {}
	SearchTuner *getTuner() {return tuner;}
	TunerRecord *changeTuner(SearchTuner *_newtuner);
	void updateTuner(SearchTuner *_newtuner) {tuner = _newtuner;}
	long long getTime(Problem *problem);
	void setTime(Problem *problem, long long time);
private:
	SearchTuner *tuner;
	Vector<Problem *> problems;
	Hashtable<Problem *, long long, uint64_t> timetaken;
	int tunernumber;
	friend class MultiTuner;
	friend void clearVector(Vector<TunerRecord *> *tunerV);
};

class MultiTuner {
public:
	MultiTuner(uint budget, uint rounds, uint timeout);
	~MultiTuner();
	void addProblem(const char *filename);
	void addTuner(SearchTuner *tuner);
	void readData(uint numRuns);
        void updateTimeout(Problem *problem, long long metric);
	void tuneK();
	void tuneComp();
	void printData();
	CMEMALLOC;
protected:
	long long evaluate(Problem *problem, TunerRecord *tuner);
	double evaluateAll(TunerRecord *tuner);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
	void mapProblemsToTuners(Vector<TunerRecord *> *tunerV);
	void improveTuners(Vector<TunerRecord *> *tunerV);
	TunerRecord *tune(TunerRecord *tuner);

	Vector<TunerRecord *> allTuners;
	Vector<Problem *> problems;
	Vector<TunerRecord *> tuners;
	uint budget;
	uint rounds;
	uint timeout;
	int execnum;
};
#endif
