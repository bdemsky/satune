#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"


Table * allocTable(Set **domains, uint numDomain, Set * range){
    Table* table = (Table*) ourmalloc(sizeof(Table));
    table->domains = allocDefVectorSet();
    for(int i=0; i<numDomain; i++){
        pushVectorSet(table->domains, domains[i]);
    }
    table->range =range;
}

void addNewTableEntry(Table* table, uint64_t* inputs, uint inputSize, uint64_t result){
    ASSERT(getSizeVectorSet( table->domains) == inputSize);
    pushVectorTableEntry(table->entries, allocTableEntry(inputs, inputSize, result));
}

