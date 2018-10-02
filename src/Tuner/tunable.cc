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

const char *tunableParameterToString(Tunables tunable) {
	switch (tunable) {
	case DECOMPOSEORDER:
		return "DECOMPOSEORDER";
	case MUSTREACHGLOBAL:
		return "MUSTREACHGLOBAL";
	case MUSTREACHLOCAL:
		return "MUSTREACHLOCAL";
	case MUSTREACHPRUNE:
		return "MUSTREACHPRUNE";
	case OPTIMIZEORDERSTRUCTURE:
		return "OPTIMIZEORDERSTRUCTURE";
	case ORDERINTEGERENCODING:
		return "ORDERINTEGERENCODING";
	case PREPROCESS:
		return "PREPROCESS";
	case NODEENCODING:
		return "NODEENCODING";
	case EDGEENCODING:
		return "EDGEENCODING";
	case MUSTEDGEPRUNE:
		return "MUSTEDGEPRUNE";
	case ELEMENTOPT:
		return "ELEMENTOPT";
	case ELEMENTOPTSETS:
		return "ELEMENTOPTSETS";
	case PROXYVARIABLE:
		return "PROXYVARIABLE";
	default:
		ASSERT(0);
	}
}
