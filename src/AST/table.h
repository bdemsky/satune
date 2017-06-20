#ifndef TABLE_H
#define TABLE_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"

struct Table {
    VectorSet* domains;
    Set * range;
    VectorTableEntry* entries;
};

Table * allocTable(Set **domains, uint numDomain, Set * range);
void addNewTableEntry(Table* table, uint64_t* inputs, uint inputSize, uint64_t result);
void deleteTable(Table* table);
#endif
