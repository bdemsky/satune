#include "randomtuner.h"
#include "csolver.h"
#include "searchtuner.h"
#include "comptuner.h"
#include <math.h>
#include <stdlib.h>
#include <limits>

#define UNSETVALUE -1

RandomTuner::RandomTuner(uint _budget, uint _timeout) : 
	BasicTuner(_budget, _timeout) {
}

RandomTuner::~RandomTuner(){
	
}

void RandomTuner::tune() {
	for (uint r = 0; r < budget; r++) {
		model_print("Round %u of %u\n", r, budget);
		for (uint i = 0; i < tuners.getSize(); i++){
			TunerRecord *tuner = tuners.get(i);
			bool isNew = true;
			for (uint j = 0; j < problems.getSize(); j++){
				Problem *problem = problems.get(j);
				long long metric = tuner->getTime(problem);
				if(metric == -1){
					metric = evaluate(problem, tuner);
					ASSERT(tuner->getTime(problem) == -1);
					tuner->addProblem(problem);
					model_print("%u.Problem<%s>\tTuner<%p, %d>\tMetric<%lld>\n", i, problem->getProblem(),tuner, tuner->getTunerNumber(), metric);
					if (metric != -1)
						tuner->setTime(problem, metric);
					else
						tuner->setTime(problem, -2);
					if(tunerExists(tuner->getTuner())){
						//Solving the problem and noticing the tuner
						//already exists
						isNew = false;
						break;
					}
				}
			}
			if(isNew){
				explored.push(tuner);
			}
			
		}
		uint tSize = tuners.getSize();
		for (uint i = 0; i < tSize; i++) {
			SearchTuner *tmpTuner = mutateTuner(tuners.get(i)->getTuner(), budget);
			while(subTunerExist(tmpTuner)){
				tmpTuner->randomMutate();
			}
			TunerRecord *tmp = new TunerRecord(tmpTuner);
			tmp->setTunerNumber(allTuners.getSize());
			model_print("Mutated tuner %u to generate tuner %u\n", tuners.get(i)->getTunerNumber(), tmp->getTunerNumber());
			allTuners.push(tmp);
			tuners.set(i, tmp);
		}
	}
	printData();
	
}
