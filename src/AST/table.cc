#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"
#include "mutableset.h"
#include "csolver.h"

Table::Table(Set **_domains, uint numDomain, Set *_range) :
	domains(_domains, numDomain),
	range(_range) {
	entries = new HashSetTableEntry();
}

void Table::addNewTableEntry(uint64_t *inputs, uint inputSize, uint64_t result) {
#ifdef CONFIG_ASSERT
	if (range == NULL)
		ASSERT(result == true || result == false);
#endif
	TableEntry *tb = allocTableEntry(inputs, inputSize, result);
	bool status = entries->add(tb);
	ASSERT(status);
}

TableEntry *Table::getTableEntry(uint64_t *inputs, uint inputSize) {
	TableEntry *temp = allocTableEntry(inputs, inputSize, -1);
	TableEntry *result = entries->get(temp);
	deleteTableEntry(temp);
	return result;
}

Table *Table::clone(CSolver *solver, CloneMap *map) {
	Table *t = (Table *) map->get(this);
	if (t != NULL)
		return t;
	Set *array[domains.getSize()];
	for (uint i = 0; i < domains.getSize(); i++) {
		array[i] = domains.get(i)->clone(solver, map);
	}
	Set *rcopy = range != NULL ? range->clone(solver, map) : NULL;
	t = solver->createTable(array, domains.getSize(), rcopy);
	SetIteratorTableEntry *entryit = entries->iterator();
	while (entryit->hasNext()) {
		TableEntry *te = entryit->next();
		solver->addTableEntry(t, &te->inputs[0], te->inputSize, te->output);
	}
	delete entryit;
	map->put(this, t);
	return t;
}

Table::~Table() {
	SetIteratorTableEntry *iterator = entries->iterator();
	while (iterator->hasNext()) {
		deleteTableEntry(iterator->next());
	}
	delete iterator;
	delete entries;
}

