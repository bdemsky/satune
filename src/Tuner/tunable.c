#include "tunable.h"

Tuner * allocTuner() {
	return ourmalloc(sizeof(Tuner));
}

void deleteTuner(Tuner *This) {
	ourfree(This);
}

int getTunable(Tuner This, TunableParam param, TunableDesc * descriptor) {
	return descriptor->defaultValue;
}
int getVarTunable(Tuner This, VarType vartype, TunableParam param, TunableDesc * descriptor) {
	return descriptor->defaultValue;
}
