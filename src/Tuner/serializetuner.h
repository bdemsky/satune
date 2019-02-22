#ifndef SERIALIZETUNER_H
#define SERIALIZETUNER_H
#include "searchtuner.h"

class SerializeTuner : public SearchTuner {
public:
	SerializeTuner(const char *filename);
	int getTunable(TunableParam param, TunableDesc *descriptor);
	int getVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor);
	CMEMALLOC;
};

#endif
