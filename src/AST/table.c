#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"


Table * allocTable(Set **domains, uint numDomain, Set * range){
    Table* table = (Table*) ourmalloc(sizeof(Table));
    table->domains = allocDefVectorSet();
    for(int i=0; i<numDomain; i++){
        pushVectorSet(table->domains, domains[i]);
    }
    table->range =range;
		return table;
}

void addNewTableEntry(Table* table, uint64_t* inputs, uint inputSize, uint64_t result){
    ASSERT(getSizeVectorSet( table->domains) == inputSize);
    pushVectorTableEntry(table->entries, allocTableEntry(inputs, inputSize, result));
}

void deleteTable(Table* table){
    uint size = getSizeVectorSet(table->domains);
    for(uint i=0; i<size; i++){
	deleteSet(getVectorSet(table->domains,i));
    }
    ourfree(table->domains);
    ourfree(table->range);
    size = getSizeVectorTableEntry(table->entries);
    for(uint i=0; i<size; i++){
	deleteTableEntry(getVectorTableEntry(table->entries, i));
    }
    ourfree(table);
}

