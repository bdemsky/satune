#include "tunable.h"

DefaultTuner::DefaultTuner() {
}

int DefaultTuner::getTunable(TunableParam param, TunableDesc *descriptor) {
	return descriptor->defaultValue;
}

int DefaultTuner::getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor) {
	return descriptor->defaultValue;
}

int DefaultTuner::getVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor) {
	return descriptor->defaultValue;
}
