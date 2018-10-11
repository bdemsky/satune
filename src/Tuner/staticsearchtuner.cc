#include "staticsearchtuner.h"
#include <iostream>
#include <fstream>
using namespace std;

StaticSearchTuner::StaticSearchTuner() {
        graphEncoding =false;
        naiveEncoding = ELEM_UNASSIGNED;
	ifstream myfile;
	myfile.open (TUNEFILE, ios::in);
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
			usedSettings.add(setting);
		}
		myfile.close();
	}
}

StaticSearchTuner *StaticSearchTuner::copyUsed() {
	StaticSearchTuner *tuner = new StaticSearchTuner();
	SetIteratorTunableSetting *iterator = usedSettings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		TunableSetting *copy = new TunableSetting(setting);
		tuner->settings.add(copy);
	}
	if(naiveEncoding != ELEM_UNASSIGNED){
		tuner->graphEncoding = graphEncoding;
		tuner->naiveEncoding = naiveEncoding;
	}
	delete iterator;
	return tuner;
}

StaticSearchTuner::~StaticSearchTuner() {
	SetIteratorTunableSetting *iterator = settings.iterator();
	while (iterator->hasNext()) {
		TunableSetting *setting = iterator->next();
		delete setting;
	}
	delete iterator;
}

int StaticSearchTuner::nextStaticTuner() {
	if(naiveEncoding == ELEM_UNASSIGNED){
		naiveEncoding = ONEHOT;
		SetIteratorTunableSetting *iter = settings.iterator();
		while(iter->hasNext()){
			TunableSetting *setting = iter->next();
			if (setting->param == NAIVEENCODER){
				setting->selectedValue = ONEHOT;
			} else if(setting->param == ENCODINGGRAPHOPT){
				setting->selectedValue = false;
			}
		}
		delete iter;
		return EXIT_FAILURE;
	}
	int result=EXIT_FAILURE;
	if(naiveEncoding == BINARYINDEX && graphEncoding){
		model_print("Best tuner\n");
		return EXIT_SUCCESS;
	}else if (naiveEncoding == BINARYINDEX && !graphEncoding){
		naiveEncoding = ONEHOT;
		graphEncoding = true;
	}else {
		naiveEncoding = (ElementEncodingType)((int)naiveEncoding + 1);
	}
	SetIteratorTunableSetting *iter = settings.iterator();
	uint count = 0;
	while(iter->hasNext()){
		TunableSetting * setting = iter->next();
		if (setting->param == NAIVEENCODER){
			setting->selectedValue = naiveEncoding;
			count++;
		} else if(setting->param == ENCODINGGRAPHOPT){
			setting->selectedValue = graphEncoding;
			count++;
		}
	}
	model_print("Mutating %u settings\n", count);
	delete iter;
	return result;
}
