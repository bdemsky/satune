#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"
#include "mutableset.h"

Table::Table(Set **_domains, uint numDomain, Set *_range) :
	domains(_domains, numDomain),
	range(_range) {
	entries = allocHashSetTableEntry(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
}

void Table::addNewTableEntry(uint64_t *inputs, uint inputSize, uint64_t result) {
#ifdef CONFIG_ASSERT
	if (range == NULL)
		ASSERT(result == true || result == false);
#endif
	TableEntry *tb = allocTableEntry(inputs, inputSize, result);
	bool status = addHashSetTableEntry(entries, tb);
	ASSERT(status);
}

TableEntry * Table::getTableEntry(uint64_t *inputs, uint inputSize) {
	TableEntry *temp = allocTableEntry(inputs, inputSize, -1);
	TableEntry *result = getHashSetTableEntry(entries, temp);
	deleteTableEntry(temp);
	return result;
}

Table::~Table() {
	HSIteratorTableEntry *iterator = iteratorTableEntry(entries);
	while (hasNextTableEntry(iterator)) {
		deleteTableEntry(nextTableEntry(iterator));
	}
	deleteIterTableEntry(iterator);
	deleteHashSetTableEntry(entries);
}

