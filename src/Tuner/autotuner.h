#ifndef AUTOTUNER_H
#define AUTOTUNER_H
#include "classlist.h"
#include "structs.h"

class AutoTuner {
 public:
	AutoTuner(uint budget);
	void addProblem(CSolver *solver);
	void tune();
	MEMALLOC;
 private:
	long long evaluate(CSolver *problem, SearchTuner *tuner);
	double evaluateAll(SearchTuner *tuner);
	SearchTuner * mutateTuner(SearchTuner * oldTuner, uint k);

	Vector<CSolver *> solvers;	
	uint budget;
};
#endif
