#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"


Table * allocTable(Set **domains, uint numDomain, Set * range){
    Table* table = (Table*) ourmalloc(sizeof(Table));
		table->numDomains=numDomain;
    table->domains = ourmalloc(numDomain*sizeof(Set *));
		memcpy(table->domains, domains, numDomain * sizeof(Set *));
    table->range =range;
		return table;
}

void addNewTableEntry(Table* table, uint64_t* inputs, uint inputSize, uint64_t result){
    ASSERT(getSizeVectorSet( table->domains) == inputSize);
    pushVectorTableEntry(table->entries, allocTableEntry(inputs, inputSize, result));
}

void deleteTable(Table* table){
	ourfree(table->domains);
	uint size = getSizeVectorTableEntry(table->entries);
	for(uint i=0; i<size; i++){
		deleteTableEntry(getVectorTableEntry(table->entries, i));
	}
	deleteVectorTableEntry(table->entries);
	ourfree(table);
}

