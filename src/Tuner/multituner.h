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
	SearchTuner *getTuner() {return tuner;}
private:
	SearchTuner *tuner;
	Vector<Problem *> problems;
	friend class MultiTuner;
	friend void clearVector(Vector<TunerRecord *> *tunerV);
};

class MultiTuner {
public:
	MultiTuner(uint budget, uint rounds, uint timeout);
	~MultiTuner();
	void addProblem(const char *filename);
	void addTuner(SearchTuner *tuner);

	void tuneK();
	CMEMALLOC;
protected:
	long long evaluate(Problem *problem, SearchTuner *tuner);
	double evaluateAll(SearchTuner *tuner, Vector<Problem *> *tProblems);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
	void mapProblemsToTuners(Vector<TunerRecord *> *tunerV);
	void improveTuners(Vector<TunerRecord *> *tunerV);
	SearchTuner *tune(SearchTuner *tuner, Vector<Problem *> *tProblems);


	Vector<Problem *> problems;
	Vector<TunerRecord *> tuners;
	uint budget;
	uint rounds;
	uint timeout;
	int execnum;
};
#endif
