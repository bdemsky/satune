#ifndef TABLE_H
#define TABLE_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"

class Table {
public:
	Table(Set **domains, uint numDomain, Set *range);
	void addNewTableEntry(uint64_t *inputs, uint inputSize, uint64_t result);
	TableEntry *getTableEntry(uint64_t *inputs, uint inputSize);
	Table *clone(CSolver *solver, CloneMap *map);
	~Table();
	Array<Set *> domains;
	Set *range;
	HashSetTableEntry *entries;
	CMEMALLOC;
};

#endif
