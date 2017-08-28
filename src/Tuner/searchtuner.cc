#include "searchtuner.h"

TunableSetting::TunableSetting(VarType _type, TunableParam _param) :
	hasVar(true),
	type(_type),
	param(_param) {
}

TunableSetting::TunableSetting(TunableParam _param) :
	hasVar(false),
	type(0),
	param(_param) {
}

void TunableSetting::setDecision(int _low, int _high, int _default, int _selection) {
	lowValue = _low;
	highValue = _high;
	defaultValue = _default;
	selectedValue = _selection;
}

unsigned int tunableSettingHash(TunableSetting *setting) {
	return setting->hasVar ^ setting->type ^ setting->param;
}

bool tunableSettingEquals(TunableSetting *setting1, TunableSetting *setting2) {
	return setting1->hasVar == setting2->hasVar &&
		setting1->type == setting2->type &&
		setting1->param == setting2->param;
}

SearchTuner::SearchTuner() {
}

SearchTuner::~SearchTuner() {
	HSIteratorTunableSetting *iterator=settings.iterator();
	while(iterator->hasNext()) {
		TunableSetting *setting=iterator->next();
		delete setting;
	}
	delete iterator;
}

int SearchTuner::getTunable(TunableParam param, TunableDesc *descriptor) {
	TunableSetting setting(param);
	TunableSetting * result = usedSettings.get(&setting);
	if (result == NULL) {
		result = settings.get(&setting);
		if ( result == NULL) {
			result=new TunableSetting(param);
			result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, descriptor->defaultValue);
			settings.add(result);
		}
		usedSettings.add(result);
	}
	return result->selectedValue;
}

int SearchTuner::getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor) {
	TunableSetting setting(vartype, param);
	TunableSetting * result = usedSettings.get(&setting);
	if (result == NULL) {
		result = settings.get(&setting);
		if ( result == NULL) {
			result=new TunableSetting(vartype, param);
			result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, descriptor->defaultValue);
			settings.add(result);
		}
		usedSettings.add(result);
	}
	return result->selectedValue;
}
