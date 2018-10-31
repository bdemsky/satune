#include "serializetuner.h"
#include <iostream>
#include <fstream>
using namespace std;

SerializeTuner::SerializeTuner(const char *filename) {
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
			usedSettings.add(setting);
		}
		myfile.close();
	} else {
		model_print("Warning: Tuner %s couldn't be loaded ... Using default tuner instead ....\n", filename);
	}
}

/*SearchTuner::~SearchTuner() {
   SetIteratorTunableSetting *iterator = settings.iterator();
   while (iterator->hasNext()) {
    TunableSetting *setting = iterator->next();
    delete setting;
   }
   delete iterator;
   }*/


int SerializeTuner::getTunable(TunableParam param, TunableDesc *descriptor) {
	TunableSetting setting(param);
	TunableSetting *result = usedSettings.get(&setting);
	if (result == NULL) {
		result = settings.get(&setting);
		if ( result == NULL) {
			return descriptor->defaultValue;
		}
		usedSettings.add(result);
	}
	return result->selectedValue;
}

int SerializeTuner::getVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor) {
	TunableSetting setting(vartype1, vartype2, param);
	TunableSetting *result = usedSettings.get(&setting);
	if (result == NULL) {
		result = settings.get(&setting);
		if ( result == NULL) {
			return descriptor->defaultValue;
		}
		usedSettings.add(result);
	}
	return result->selectedValue;
}
