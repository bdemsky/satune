#ifndef RANDOMTUNER_H
#define RANDOMTUNER_H
#include "classlist.h"
#include "structs.h"
#include "multituner.h"

/**
 * This is a Tuner which is being used for 
 */
class RandomTuner {
public:
	RandomTuner(uint _budget, uint _timeout);
	~RandomTuner();
	void addProblem(const char *filename);
	void addTuner(SearchTuner *tuner);
	void tune();
	void printData();
	CMEMALLOC;
protected:
	long long evaluate(Problem *problem, TunerRecord *tuner);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
        void updateTimeout(Problem *problem, long long metric);
        bool randomMutation(SearchTuner *tuner);
        bool tunerExists(SearchTuner *tunerRec);
	/**
         * returns the index of the tuner which is subtune of
         * the newTuner 
         * @param newTuner
         * @return 
         */
        int subtuneIndex(SearchTuner *newTuner);
        
	TunerRecord *tune(SearchTuner *tuner);
	Vector<TunerRecord *> allTuners;
	Vector<TunerRecord *> explored;
	Vector<Problem *> problems;
	Vector<TunerRecord *> tuners;
	uint budget;
	uint timeout;
	int execnum;
};

#endif
