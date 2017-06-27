#ifndef TABLE_H
#define TABLE_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"

struct Table {
	ArrayElement domains;
	Element * range;
	VectorTableEntry entries;
};

Table * allocTable(Element **domains, uint numDomain, Element * range);
void addNewTableEntry(Table* table, uint64_t* inputs, uint inputSize, uint64_t result);
void deleteTable(Table* table);
#endif
