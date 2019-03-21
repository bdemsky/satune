/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   kmeanstuner.h
 * Author: hamed
 *
 * Created on December 19, 2018, 4:16 PM
 */

#ifndef KMEANSTUNER_H
#define KMEANSTUNER_H

#include "basictuner.h"


class KMeansTuner : public BasicTuner {
public:
	KMeansTuner(uint budget, uint rounds, uint timeout );
	virtual ~KMeansTuner();
	void tune();
private:
	double evaluateAll(TunerRecord *tuner);
	void mapProblemsToTuners(Vector<TunerRecord *> *tunerV);
	void improveTuners(Vector<TunerRecord *> *tunerV);
	TunerRecord *tune(TunerRecord *tuner);
	uint rounds;
};

#endif/* KMEANSTUNER_H */

