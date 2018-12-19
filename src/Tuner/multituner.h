#ifndef MULTITUNER_H
#define MULTITUNER_H
#include "classlist.h"
#include "structs.h"
#include "basictuner.h"


class MultiTuner : public BasicTuner {
public:
	MultiTuner(uint budget, uint rounds, uint timeout);
	void readData(uint numRuns);
	void tuneK();
	void tune();
	void findBestThreeTuners();
protected:
	double evaluateAll(TunerRecord *tuner);
	void mapProblemsToTuners(Vector<TunerRecord *> *tunerV);
	void improveTuners(Vector<TunerRecord *> *tunerV);
	TunerRecord *tune(TunerRecord *tuner);
	uint rounds;
};

inline long long min(long long num1, long long num2, long long num3) {
	return num1 < num2 && num1 < num3 ? num1 :
				 num2 < num3 ? num2 : num3;
}

#endif
