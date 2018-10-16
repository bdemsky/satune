#ifndef SEARCHTUNER_H
#define SEARCHTUNER_H
#include "classlist.h"
#include "tunable.h"
#include "structs.h"
#include <ostream>
using namespace std;

class TunableSetting {
public:
	TunableSetting(VarType type, TunableParam param);
	TunableSetting(VarType type1, VarType type2, TunableParam param);
	TunableSetting(TunableParam param);
	TunableSetting(TunableSetting *ts);
	void setDecision(int _low, int _high, int _default, int _selection);
	void print();
	friend std ::ostream &operator<< (std::ostream &stream, const TunableSetting &matrix);
	CMEMALLOC;
private:
	bool hasVar;
	VarType type1;
	VarType type2;
	TunableParam param;
	int lowValue;
	int highValue;
	int defaultValue;
	int selectedValue;
	friend unsigned int tunableSettingHash(TunableSetting *setting);
	friend bool tunableSettingEquals(TunableSetting *setting1, TunableSetting *setting2);
	friend class SearchTuner;
	friend class StaticSearchTuner;
};

unsigned int tunableSettingHash(TunableSetting *setting);
bool tunableSettingEquals(TunableSetting *setting1, TunableSetting *setting2);

typedef Hashset<TunableSetting *, uintptr_t, 4, tunableSettingHash, tunableSettingEquals> HashsetTunableSetting;
typedef SetIterator<TunableSetting *, uintptr_t, 4, tunableSettingHash, tunableSettingEquals> SetIteratorTunableSetting;

class SearchTuner : public Tuner {
public:
	SearchTuner();
	SearchTuner(const char *filename);
	~SearchTuner();
	int getTunable(TunableParam param, TunableDesc *descriptor);
	int getVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor);
	int getVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor);
	void setTunable(TunableParam param, TunableDesc *descriptor, uint value);
	void setVarTunable(VarType vartype, TunableParam param, TunableDesc *descriptor, uint value);
	void setVarTunable(VarType vartype1, VarType vartype2, TunableParam param, TunableDesc *descriptor, uint value);
	SearchTuner *copyUsed();
	void randomMutate();
	uint getSize() { return usedSettings.getSize();}
	void print();
	void printUsed();
	void serialize(const char *file);
	void serializeUsed(const char *file);
	void addUsed(const char *file);

	CMEMALLOC;
protected:
	/** Used Settings keeps track of settings that were actually used by
	   the example. Mutating settings may cause the Constraint Compiler
	   not to query other settings.*/
	HashsetTunableSetting usedSettings;
	/** Settings contains all settings. */
	HashsetTunableSetting settings;
};

#endif
