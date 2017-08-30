#ifndef SEARCHTUNER_H
#define SEARCHTUNER_H
#include "classlist.h"
#include "tunable.h"
#include "structs.h"

class TunableSetting {
 public:
	TunableSetting(VarType type, TunableParam param);	
	TunableSetting(TunableParam param);
	TunableSetting(TunableSetting * ts);
	void setDecision(int _low, int _high, int _default, int _selection);
	void print();
	CMEMALLOC;
 private:
	bool hasVar;
	VarType type;
	TunableParam param;
	int lowValue;
	int highValue;
	int defaultValue;
	int selectedValue;
	friend unsigned int tunableSettingHash(TunableSetting *setting);
	friend bool tunableSettingEquals(TunableSetting *setting1, TunableSetting *setting2);
	friend class SearchTuner;
};

unsigned int tunableSettingHash(TunableSetting *setting);
bool tunableSettingEquals(TunableSetting *setting1, TunableSetting *setting2);

typedef HashSet<TunableSetting *, uintptr_t, 4, tunableSettingHash, tunableSettingEquals> HashSetTunableSetting;
typedef HSIterator<TunableSetting *, uintptr_t, 4, tunableSettingHash, tunableSettingEquals> HSIteratorTunableSetting;

class SearchTuner : public Tuner {
 public:
	SearchTuner();
	~SearchTuner();
	int getTunable(TunableParam param, TunableDesc *descriptor);
	int getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor);
	SearchTuner * copyUsed();
	void randomMutate();
	uint getSize() { return usedSettings.getSize();}
	void print();
	void printUsed();

	CMEMALLOC;
 private:
	/** Used Settings keeps track of settings that were actually used by
		 the example. Mutating settings may cause the Constraint Compiler
		 not to query other settings.*/
	HashSetTunableSetting usedSettings;
	/** Settings contains all settings. */
	HashSetTunableSetting settings;
};
#endif
