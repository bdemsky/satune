#ifndef AUTOTUNER_H
#define AUTOTUNER_H
#include "classlist.h"
#include "structs.h"

class AutoTuner {
 public:
	AutoTuner();
	void addProblem(CSolver *solver);
	void tune();
	MEMALLOC;
 private:
	Vector<CSolver *> solvers;
};
#endif
