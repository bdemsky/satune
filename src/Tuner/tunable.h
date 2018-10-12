#ifndef TUNABLE_H
#define TUNABLE_H
#include "classlist.h"
#include "common.h"

class Tuner {
public:
	virtual int getTunable(TunableParam param, TunableDesc *descriptor) {ASSERT(0); return 0;}
	virtual int getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor) {ASSERT(0); return 0;}
	virtual int getVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor) {ASSERT(0); return 0;}
	virtual ~Tuner() {}
	CMEMALLOC;
};

class DefaultTuner : public Tuner {
public:
	DefaultTuner();
	int getTunable(TunableParam param, TunableDesc *descriptor);
	int getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor);
	int getVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor);
	CMEMALLOC;
};



class TunableDesc {
public:
	TunableDesc(int _lowValue, int _highValue, int _defaultValue) : lowValue(_lowValue), highValue(_highValue), defaultValue(_defaultValue) {}
	int lowValue;
	int highValue;
	int defaultValue;
	CMEMALLOC;
};


#define GETTUNABLE(This, param, descriptor) This->getTunable(param, descriptor)
#define GETVARTUNABLE(This, vartype, param, descriptor) This->getTunable(param, descriptor)

static TunableDesc onoff(0, 1, 1);
static TunableDesc offon(0, 1, 0);
static TunableDesc proxyparameter(1, 5, 2);
static TunableDesc mustValueBinaryIndex(5, 9, 8);
static TunableDesc NodeEncodingDesc(ELEM_UNASSIGNED, BINARYINDEX, ELEM_UNASSIGNED);
static TunableDesc NaiveEncodingDesc(ONEHOT, BINARYINDEX, ONEHOT);
static TunableDesc boolVarOrderingDesc(CONSTRAINTORDERING, REVERSEORDERING, REVERSEORDERING);

enum Tunables {DECOMPOSEORDER, MUSTREACHGLOBAL, MUSTREACHLOCAL, MUSTREACHPRUNE, OPTIMIZEORDERSTRUCTURE, ORDERINTEGERENCODING, PREPROCESS, NODEENCODING, EDGEENCODING, MUSTEDGEPRUNE, ELEMENTOPT, 
        ENCODINGGRAPHOPT, ELEMENTOPTSETS, PROXYVARIABLE, MUSTVALUE, NAIVEENCODER, VARIABLEORDER};
typedef enum Tunables Tunables;

const char *tunableParameterToString(Tunables tunable);
#endif
