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
	int result;
	char *problem;
	friend class MultiTuner;
};

class TunerRecord {
public:
	TunerRecord(SearchTuner *_tuner) : tuner(_tuner) {}

private:
	SearchTuner *tuner;
	friend class MultiTuner;
};

class MultiTuner {
public:
	MultiTuner(uint budget, uint timeout);
	~MultiTuner();
	void addProblem(const char *filename);
	void addTuner(SearchTuner *tuner);
	virtual void tune();
	CMEMALLOC;
protected:
	long long evaluate(Problem *problem, SearchTuner *tuner);
	double evaluateAll(SearchTuner *tuner);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
	Vector<Problem *> problems;
	Vector<TunerRecord *> tuners;
	uint budget;
	uint timeout;
	int execnum;
};
#endif
