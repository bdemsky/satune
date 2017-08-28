#ifndef AUTOTUNER_H
#define AUTOTUNER_H
#include "classlist.h"

class AutoTuner {
 public:
	AutoTuner();
	void tune(CSolver *solver);
	MEMALLOC;
 private:
};
#endif
