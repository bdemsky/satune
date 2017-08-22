#ifndef TUNABLE_H
#define TUNABLE_H
#include "classlist.h"


struct Tuner {
};

struct TunableDesc {
	int lowValue;
	int highValue;
	int defaultValue;
};

Tuner * allocTuner();
void deleteTuner(Tuner *This);

int getTunable(Tuner *This, TunableParam param, TunableDesc * descriptor);
int getVarTunable(Tuner *This, VarType vartype, TunableParam param, TunableDesc * descriptor);

#define GETTUNABLE(This, param, descriptor) getTunable(This, param, descriptor)
#define GETVARTUNABLE(This, vartype, param, descriptor) getTunable(This, param, descriptor)

enum Tunables {DECOMPOSEORDER, MUSTREACHGLOBAL, MUSTREACHLOCAL, MUSTREACHPRUNE};
typedef enum Tunables Tunables;
#endif
