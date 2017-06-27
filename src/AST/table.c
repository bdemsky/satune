#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"
#include "mutableset.h"


Table * allocTable(Element **domains, uint numDomain, Element * range){
	Table* table = (Table*) ourmalloc(sizeof(Table));
	allocInlineArrayInitElement(&table->domains, domains, numDomain);
	allocInlineDefVectorTableEntry(&table->entries);
	table->range =range;
	return table;
}

void addNewTableEntry(Table* table, uint64_t* inputs, uint inputSize, uint64_t result){
	ASSERT(getSizeArrayElement( &table->domains) == inputSize);
	pushVectorTableEntry(&table->entries, allocTableEntry(inputs, inputSize, result));
}

void deleteTable(Table* table){
  deleteInlineArrayElement(&table->domains);
  uint size = getSizeVectorTableEntry(&table->entries);
  for(uint i=0; i<size; i++){
    deleteTableEntry(getVectorTableEntry(&table->entries, i));
  }
  deleteVectorArrayTableEntry(&table->entries);
  ourfree(table);
}

