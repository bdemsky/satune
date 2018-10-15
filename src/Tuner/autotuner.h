#ifndef AUTOTUNER_H
#define AUTOTUNER_H
#include "classlist.h"
#include "structs.h"

#define AUTOTUNERFACTOR 0.3


class AutoTuner {
public:
	AutoTuner(uint budget);
	virtual ~AutoTuner();
	void addProblem(CSolver *solver);
	virtual void tune();
	CMEMALLOC;
protected:
	long long evaluate(CSolver *problem, SearchTuner *tuner);
	double evaluateAll(SearchTuner *tuner);
	SearchTuner *mutateTuner(SearchTuner *oldTuner, uint k);
	Vector<CSolver *> solvers;
	uint budget;
	int result;
};
#endif
