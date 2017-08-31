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

TunableSetting::TunableSetting(TunableSetting * ts) :
	hasVar(ts->hasVar),
	type(ts->type),
	param(ts->param),
  lowValue(ts->lowValue),
	highValue(ts->highValue),
	defaultValue(ts->defaultValue),
	selectedValue(ts->selectedValue)
{
}

void TunableSetting::setDecision(int _low, int _high, int _default, int _selection) {
	lowValue = _low;
	highValue = _high;
	defaultValue = _default;
	selectedValue = _selection;
}

void TunableSetting::print() {
	if (hasVar) {
		model_print("Type %" PRIu64 ", ", type);
	}
	model_print("Param %u = %u\n", param, selectedValue);
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

SearchTuner * SearchTuner::copyUsed() {
	SearchTuner * tuner = new SearchTuner();
	SetIteratorTunableSetting *iterator=usedSettings.iterator();
	while(iterator->hasNext()) {
		TunableSetting *setting=iterator->next();
		TunableSetting *copy=new TunableSetting(setting);
		tuner->settings.add(copy);
	}
	delete iterator;
	return tuner;
}

SearchTuner::~SearchTuner() {
	SetIteratorTunableSetting *iterator=settings.iterator();
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
			uint value = descriptor->lowValue + (random() % (1+ descriptor->highValue - descriptor->lowValue));
			result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, value);
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
			uint value = descriptor->lowValue + (random() % (1+ descriptor->highValue - descriptor->lowValue));
			result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, value);
			settings.add(result);
		}
		usedSettings.add(result);
	}
	return result->selectedValue;
}

void SearchTuner::randomMutate() {
	TunableSetting * randomSetting = settings.getRandomElement();
	int range=randomSetting->highValue-randomSetting->lowValue;
	int randomchoice=(random() % range) + randomSetting->lowValue;
	if (randomchoice < randomSetting->selectedValue)
		randomSetting->selectedValue = randomchoice;
	else
		randomSetting->selectedValue = randomchoice + 1;
}

void SearchTuner::print() {
	SetIteratorTunableSetting *iterator=settings.iterator();
	while(iterator->hasNext()) {
		TunableSetting *setting=iterator->next();
		setting->print();
	}
	delete iterator;

}

void SearchTuner::printUsed() {
	SetIteratorTunableSetting *iterator=usedSettings.iterator();
	while(iterator->hasNext()) {
		TunableSetting *setting=iterator->next();
		setting->print();
	}
	delete iterator;
}
