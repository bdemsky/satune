#ifndef STATICAUTOTUNER_H
#define STATICAUTOTUNER_H
#include "autotuner.h"


class StaticAutoTuner : public AutoTuner {
public:
	StaticAutoTuner(uint budget);
  virtual void tune();
  	CMEMALLOC;
private:
	StaticSearchTuner *mutateTuner(StaticSearchTuner *oldTuner);
};
#endif
