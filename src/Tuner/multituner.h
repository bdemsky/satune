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
	long long besttime;
	friend class MultiTuner;
        friend class RandomTuner;
};

class TunerRecord {
public:
	TunerRecord(SearchTuner *_tuner) : tuner(_tuner), tunernumber(-1) {}
	TunerRecord(SearchTuner *_tuner, int _tunernumber) : tuner(_tuner), tunernumber(_tunernumber) {}
	SearchTuner *getTuner() {return tuner;}
	TunerRecord *changeTuner(SearchTuner *_newtuner);
	void updateTuner(SearchTuner *_newtuner) {tuner = _newtuner;}
	long long getTime(Problem *problem);
	void setTime(Problem *problem, long long time);
        void print();
private:
	SearchTuner *tuner;
	Vector<Problem *> problems;
	Hashtable<Problem *, long long, uint64_t> timetaken;
	int tunernumber;
	friend class MultiTuner;
        friend class RandomTuner;
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
	bool finishTunerExist(TunerRecord *tuner);
	void tuneComp();
	void printData();
	void findBestThreeTuners();
	CMEMALLOC;
protected:
	long long evaluate(Problem *problem, TunerRecord *tuner);
	double evaluateAll(TunerRecord *tuner);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
	bool hasExplored(SearchTuner *newTuner);
	void mapProblemsToTuners(Vector<TunerRecord *> *tunerV);
	void improveTuners(Vector<TunerRecord *> *tunerV);
	TunerRecord *tune(TunerRecord *tuner);
	Vector<TunerRecord *> allTuners;
	Vector<TunerRecord *> explored;
	Vector<Problem *> problems;
	Vector<TunerRecord *> tuners;
	uint budget;
	uint rounds;
	uint timeout;
	int execnum;
};

inline long long min(long long num1, long long num2, long long num3) {
	return num1 < num2 && num1 < num3 ? num1 :
				 num2 < num3 ? num2 : num3;
}

#endif
