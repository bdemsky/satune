#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"
#include "mutableset.h"

Table * allocTable(Set **domains, uint numDomain, Set * range){
	Table* This = (Table*) ourmalloc(sizeof(Table));
	initArrayInitSet(&This->domains, domains, numDomain);
	initDefVectorTableEntry(&This->entries);
	This->range =range;
	return This;
}

void addNewTableEntry(Table* This, uint64_t* inputs, uint inputSize, uint64_t result){
	ASSERT(getSizeArraySet( &This->domains) == inputSize);
	pushVectorTableEntry(&This->entries, allocTableEntry(inputs, inputSize, result));
}

void deleteTable(Table* This){
  deleteInlineArraySet(&This->domains);
  uint size = getSizeVectorTableEntry(&This->entries);
  for(uint i=0; i<size; i++){
    deleteTableEntry(getVectorTableEntry(&This->entries, i));
  }
  deleteVectorArrayTableEntry(&This->entries);
  ourfree(This);
}

