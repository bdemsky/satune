#ifndef SATUNER_H
#define SATUNER_H
#include "classlist.h"
#include "structs.h"
#include "basictuner.h"

/**
*This tuner has the simulated annealing in its core
*
*/
class SATuner : public BasicTuner {
public:
	SATuner(uint budget, uint timeout);
	virtual ~SATuner();
	void tune();
protected:
	void insertInPlace(Vector<TunerRecord *> *places, TunerRecord *tuner, Problem *problem, long long metric);
	void initialize(Vector<TunerRecord *> *tunerV, Vector<Vector<TunerRecord *> *> &allplaces);
	void rankTunerForProblem(Vector<TunerRecord *> *places, TunerRecord *tuner, Problem *problem, long long metric);
	void removeTunerIndex(Vector<TunerRecord *> *tunerV, int index,  Vector<Vector<TunerRecord *> *> &allplaces);
	void removeNullsFromTunerVector( Vector<TunerRecord *> *tunerV);
};


#endif
