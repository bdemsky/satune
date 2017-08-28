#ifndef SEARCHTUNER_H
#define SEARCHTUNER_H
#include "classlist.h"
#include "tunable.h"

class SearchTuner : public Tuner {
 public:
	SearchTuner();
	int getTunable(TunableParam param, TunableDesc *descriptor);
	int getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor);
	MEMALLOC;
 private:
};

class TunableParameter {
 public:
	TunableParameter(VarType type, TunableParam param);	
	TunableParameter(TunableParam param);
	MEMALLOC;
 private:
	bool hasVar;
	VarType type;
	TunableParam param;
};

class TunableDecision {
 public:
	TunableDecision(int _low, int _high, int _default, int _selection);
	MEMALLOC;
 private:
	int lowValue;
	int highValue;
	int defaultValue;
	int selectedValue;
};

#endif
