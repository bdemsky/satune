#ifndef TABLE_H
#define TABLE_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"

class Table {
public:
	Table(Set *range);
	void addNewTableEntry(uint64_t *inputs, uint inputSize, uint64_t result);
	TableEntry *getTableEntry(uint64_t *inputs, uint inputSize);
	Table *clone(CSolver *solver, CloneMap *map);
	void serialize(Serializer *serializer);
	void print();
	~Table();
	Set *getRange() {return range;}

	SetIteratorTableEntry *getEntries() {return entries->iterator();}
	uint getSize() {return entries->getSize();}

	CMEMALLOC;

private:
	Set *range;
	HashsetTableEntry *entries;
};

#endif
