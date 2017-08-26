#include "tunable.h"

Tuner::Tuner() {
}

int Tuner::getTunable(TunableParam param, TunableDesc *descriptor) {
	return descriptor->defaultValue;
}
int Tuner::getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor) {
	return descriptor->defaultValue;
}
