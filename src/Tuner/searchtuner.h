#ifndef SEARCHTUNER_H
#define SEARCHTUNER_H
#include "classlist.h"
#include "tunable.h"
#include "structs.h"

class TunableSetting {
 public:
	TunableSetting(VarType type, TunableParam param);	
	TunableSetting(TunableParam param);
	void setDecision(int _low, int _high, int _default, int _selection);
	MEMALLOC;
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
	MEMALLOC;
 private:
	HashSetTunableSetting usedSettings;
	HashSetTunableSetting settings;
};

#endif
