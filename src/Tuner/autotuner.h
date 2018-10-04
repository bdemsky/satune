#ifndef AUTOTUNER_H
#define AUTOTUNER_H
#include "classlist.h"
#include "structs.h"

#define AUTOTUNERFACTOR 0.3


class AutoTuner {
public:
	AutoTuner(uint budget);
	void addProblem(CSolver *solver);
	void tune();
	CMEMALLOC;
private:
	long long evaluate(CSolver *problem, SearchTuner *tuner);
	double evaluateAll(SearchTuner *tuner);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
#ifdef STATICENCGEN
	SearchTuner *mutateTuner(SearchTuner *oldTuner);
#endif
	Vector<CSolver *> solvers;
	uint budget;
	int result;
};
#endif
