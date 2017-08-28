#include "searchtuner.h"

SearchTuner::SearchTuner() {
}

int SearchTuner::getTunable(TunableParam param, TunableDesc *descriptor) {
	return 0;
}

int SearchTuner::getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor) {
	return 0;
}

TunableParameter::TunableParameter(VarType _type, TunableParam _param) :
	hasVar(true),
	type(_type),
	param(_param) {
}

TunableParameter::TunableParameter(TunableParam _param) :
	hasVar(false),
	type(0),
	param(_param) {
}

TunableDecision::TunableDecision(int _low, int _high, int _default, int _selection) :
	lowValue(_low),
	highValue(_high),
	defaultValue(_default),
	selectedValue(_selection) {
}
