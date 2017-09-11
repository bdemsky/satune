#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"
#include "mutableset.h"
#include "csolver.h"
#include "serializer.h"

Table::Table(Set **_domains, uint numDomain, Set *_range) :
	domains(_domains, numDomain),
	range(_range) {
	entries = new HashsetTableEntry();
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



void Table::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	
	uint size = domains.getSize();
	for(uint i=0; i<size; i++){
		Set* domain = domains.get(i);
		domain->serialize(serializer);
	}
	if(range!= NULL)
		range->serialize(serializer);
	
	ASTNodeType type = TABLETYPE;	
	serializer->mywrite(&type, sizeof(ASTNodeType));
	Table* This = this;
	serializer->mywrite(&This, sizeof(Table*));
	serializer->mywrite(&size, sizeof(uint));
	for(uint i=0; i<size; i++){
		Set* domain = domains.get(i);
		serializer->mywrite(&domain, sizeof(Set*));
	}
	serializer->mywrite(&range, sizeof(Set*));
	size = entries->getSize();
	serializer->mywrite(&size, sizeof(uint));
	SetIteratorTableEntry* iterator = getEntries();
	while(iterator->hasNext()){
		TableEntry* entry = iterator->next();
		serializer->mywrite(&entry->output, sizeof(uint64_t));
		serializer->mywrite(&entry->inputSize, sizeof(uint));
		serializer->mywrite(entry->inputs, sizeof(uint64_t) * entry->inputSize);
	}
}


