#include "searchtuner.h"
#include <iostream>
#include <fstream>
using namespace std;

TunableSetting::TunableSetting(VarType _type, TunableParam _param) :
	hasVar(true),
	type1(_type),
	type2(0),
	param(_param) {
}

TunableSetting::TunableSetting(VarType _type1, VarType _type2, TunableParam _param) :
	hasVar(true),
	type1(_type1),
	type2(_type2),
	param(_param) {
}

TunableSetting::TunableSetting(TunableParam _param) :
	hasVar(false),
	type1(0),
	type2(0),
	param(_param) {
}

TunableSetting::TunableSetting(TunableSetting *ts) :
	hasVar(ts->hasVar),
	type1(ts->type1),
	type2(ts->type2),
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
	model_print("Param %s = %u \t range=[%u,%u]", tunableParameterToString( (Tunables)param), selectedValue, lowValue, highValue);
	if (hasVar) {
		model_print("\tVarType1 %" PRIu64 ", ", type1);
		model_print("VarType2 %" PRIu64 ", ", type2);
	}
	model_print("\n");
}


ostream &operator<<(ostream &os, const TunableSetting &ts)
{
	os << ts.hasVar << " " << ts.type1 << " " << ts.type2 << " " << ts.param << " " << ts.lowValue << " "
		 << ts.highValue << " " << ts.defaultValue << " " << ts.selectedValue;
	return os;
}


SearchTuner::SearchTuner() {
}

SearchTuner::SearchTuner(const char *filename, bool addused) {
	ifstream myfile;
	myfile.open (filename, ios::in);
	if (myfile.is_open()) {
		bool hasVar;
		VarType type1;
		VarType type2;
		TunableParam param;
		int lowValue;
		int highValue;
		int defaultValue;
		int selectedValue;
		while (myfile >> hasVar >> type1 >> type2 >> param >> lowValue >> highValue >> defaultValue >> selectedValue) {
			TunableSetting *setting;

			if (hasVar) {
				setting = new TunableSetting(type1, type2, param);
			} else {
				setting = new TunableSetting(param);
			}
			setting->setDecision(lowValue, highValue, defaultValue, selectedValue);
			settings.add(setting);
			if (addused) {
				usedSettings.add(setting);
			}
		}
		myfile.close();
	} else {
		model_print("Warning: Tuner %s couldn't be loaded ... Using default tuner instead ....\n", filename);
	}
}

bool SearchTuner::equalUsed(SearchTuner *tuner) {
	if (tuner->usedSettings.getSize() != usedSettings.getSize()) {
		return false;
	}
	bool result = true;
	SetIteratorTunableSetting *iterator = usedSettings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		if (!tuner->usedSettings.contains(setting)) {
			result = false;
			break;
		} else {
			TunableSetting *tunerSetting = tuner->usedSettings.get(setting);
			if (tunerSetting->selectedValue != setting->selectedValue) {
				result = false;
				break;
			}
		}
	}
	delete iterator;
	return result;
}

void SearchTuner::addUsed(const char *filename) {
	ifstream myfile;
	myfile.open (filename, ios::in);
	if (myfile.is_open()) {
		bool hasVar;
		VarType type1;
		VarType type2;
		TunableParam param;
		int lowValue;
		int highValue;
		int defaultValue;
		int selectedValue;
		while (myfile >> hasVar >> type1 >> type2 >> param >> lowValue >> highValue >> defaultValue >> selectedValue) {
			TunableSetting *setting;

			if (hasVar) {
				setting = new TunableSetting(type1, type2, param);
			} else {
				setting = new TunableSetting(param);
			}
			setting->setDecision(lowValue, highValue, defaultValue, selectedValue);
			if (!settings.contains(setting)) {
				settings.add(setting);
				usedSettings.add(setting);
			} else {
				TunableSetting *tmp = settings.get(setting);
				settings.remove(tmp);
				usedSettings.remove(tmp);
				delete tmp;
				settings.add(setting);
				usedSettings.add(setting);
			}
		}
		myfile.close();
	}
}

bool SearchTuner::isSubTunerof(SearchTuner *newTuner) {
	SetIteratorTunableSetting *iterator = usedSettings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		if (!newTuner->settings.contains(setting)) {
			return false;
		} else {
			TunableSetting *newSetting = newTuner->settings.get(setting);
			if (newSetting->selectedValue != setting->selectedValue) {
				return false;
			}
		}
	}
	delete iterator;
	return true;
}

SearchTuner *SearchTuner::copyUsed() {
	SearchTuner *tuner = new SearchTuner();
	SetIteratorTunableSetting *iterator = usedSettings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		TunableSetting *copy = new TunableSetting(setting);
		tuner->settings.add(copy);
	}
	delete iterator;
	return tuner;
}

SearchTuner::~SearchTuner() {
	SetIteratorTunableSetting *iterator = settings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		delete setting;
	}
	delete iterator;
}

void SearchTuner::setTunable(TunableParam param, TunableDesc *descriptor, uint value) {
	TunableSetting *result = new TunableSetting(param);
	result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, value);
	settings.add(result);
	usedSettings.add(result);
}

void SearchTuner::setVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor, uint value) {
	setVarTunable(vartype, 0, param, descriptor, value);
}

void SearchTuner::setVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor, uint value) {
	TunableSetting *result = new TunableSetting(vartype1, vartype2, param);
	result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, value);
	settings.add(result);
	usedSettings.add(result);
}

int SearchTuner::getTunable(TunableParam param, TunableDesc *descriptor) {
	TunableSetting setting(param);
	TunableSetting *result = usedSettings.get(&setting);
	if (result == NULL) {
		result = settings.get(&setting);
		if ( result == NULL) {
			result = new TunableSetting(param);
			uint value = descriptor->lowValue + (random() % (1 + descriptor->highValue - descriptor->lowValue));
			result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, value);
			settings.add(result);
		}
		usedSettings.add(result);
	}
	return result->selectedValue;
}

int SearchTuner::getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor) {
	return getVarTunable(vartype, 0, param, descriptor);
}

int SearchTuner::getVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor) {
	TunableSetting setting(vartype1, vartype2, param);
	TunableSetting *result = usedSettings.get(&setting);
	if (result == NULL) {
		result = settings.get(&setting);
		if ( result == NULL) {
			result = new
							 TunableSetting(vartype1, vartype2, param);
			uint value = descriptor->lowValue + (random() % (1 + descriptor->highValue - descriptor->lowValue));
			result->setDecision(descriptor->lowValue, descriptor->highValue, descriptor->defaultValue, value);
			settings.add(result);
		}
		usedSettings.add(result);
	}
	return result->selectedValue;
}

void SearchTuner::randomMutate() {
	TunableSetting *randomSetting = settings.getRandomElement();
	int range = randomSetting->highValue - randomSetting->lowValue;
	int randomchoice = (random() % range) + randomSetting->lowValue;
	if (randomchoice < randomSetting->selectedValue)
		randomSetting->selectedValue = randomchoice;
	else
		randomSetting->selectedValue = randomchoice + 1;
	model_print("&&&&&&&&Mutating&&&&&&&\n");
	randomSetting->print();
	model_print("&&&&&&&&&&&&&&&&&&&&&&&\n");
}

void SearchTuner::print() {
	SetIteratorTunableSetting *iterator = settings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		setting->print();
	}
	delete iterator;

}

void SearchTuner::serialize(const char *filename) {
	ofstream myfile;
	myfile.open (filename, ios::out | ios::trunc);
	SetIteratorTunableSetting *iterator = settings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		myfile << *setting << endl;
	}
	myfile.close();
	delete iterator;
}

void SearchTuner::serializeUsed(const char *filename) {
	ofstream myfile;
	myfile.open (filename, ios::out | ios::trunc);
	SetIteratorTunableSetting *iterator = usedSettings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		myfile << *setting << endl;
	}
	myfile.close();
	delete iterator;
}

void SearchTuner::printUsed() {
	SetIteratorTunableSetting *iterator = usedSettings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		setting->print();
	}
	delete iterator;
}
