#ifndef RANDOMTUNER_H
#define RANDOMTUNER_H
#include "classlist.h"
#include "structs.h"
#include "comptuner.h"
#include "basictuner.h"

/**
 * This is a Tuner which is being used for 
 */
class RandomTuner : public BasicTuner {
public:
	RandomTuner(uint _budget, uint _timeout);
	virtual ~RandomTuner();
	void tune();
protected:
        bool randomMutation(SearchTuner *tuner);        
	TunerRecord *tune(SearchTuner *tuner);
};

#endif
