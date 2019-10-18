#ifndef COMPTUNER_H
#define COMPTUNER_H
#include "classlist.h"
#include "structs.h"
#include "basictuner.h"


class CompTuner : public BasicTuner {
public:
	CompTuner(uint budget, uint timeout);
	virtual ~CompTuner();
	void readData(uint numRuns);
	void tune();
	void findBestTwoTuners();
protected:

};

inline long long min(long long num1, long long num2, long long num3) {
	return num1 < num2 && num1 < num3 ? num1 :
				 num2 < num3 ? num2 : num3;
}

#endif
