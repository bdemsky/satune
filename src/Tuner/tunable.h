#ifndef TUNABLE_H
#define TUNABLE_H
#include "classlist.h"


class Tuner {
 public:
	Tuner();
	int getTunable(TunableParam param, TunableDesc * descriptor);
	int getVarTunable(VarType vartype, TunableParam param, TunableDesc * descriptor);
	MEMALLOC;
};

class TunableDesc {
 public:
 TunableDesc(int _lowValue, int _highValue, int _defaultValue) : lowValue(_lowValue), highValue(_highValue), defaultValue(_defaultValue) {}
	int lowValue;
	int highValue;
	int defaultValue;
	MEMALLOC;
};


#define GETTUNABLE(This, param, descriptor) This->getTunable(param, descriptor)
#define GETVARTUNABLE(This, vartype, param, descriptor) This->getTunable(param, descriptor)

static TunableDesc onoff(0, 1, 1);
static TunableDesc offon(0, 1, 0);

enum Tunables {DECOMPOSEORDER, MUSTREACHGLOBAL, MUSTREACHLOCAL, MUSTREACHPRUNE, OPTIMIZEORDERSTRUCTURE, ORDERINTEGERENCODING};
typedef enum Tunables Tunables;
#endif
