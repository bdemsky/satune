#include "table.h"
#include "common.h"
#include "structs.h"
#include "tableentry.h"
#include "set.h"
#include "mutableset.h"

Table * allocTable(Set **domains, uint numDomain, Set * range){
	Table* This = (Table*) ourmalloc(sizeof(Table));
	initArrayInitSet(&This->domains, domains, numDomain);
	This->entries= allocHashSetTableEntry(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	This->range =range;
	return This;
}

void addNewTableEntry(Table* This, uint64_t* inputs, uint inputSize, uint64_t result){
	ASSERT(getSizeArraySet( &This->domains) == inputSize);
#ifdef CONFIG_ASSERT
	if(This->range==NULL)
		ASSERT(result == true || result == false);
#endif
	TableEntry* tb = allocTableEntry(inputs, inputSize, result);
	ASSERT(!containsHashSetTableEntry(This->entries, tb));
	bool status= addHashSetTableEntry(This->entries, tb);
	ASSERT(status);
}

TableEntry* getTableEntryFromTable(Table* table, uint64_t* inputs, uint inputSize){
	TableEntry* temp = allocTableEntry(inputs, inputSize, -1);
	TableEntry* result= getHashSetTableEntry(table->entries, temp);
	deleteTableEntry(temp);
	return result;
}

void deleteTable(Table* This){
  deleteInlineArraySet(&This->domains);
  HSIteratorTableEntry* iterator = iteratorTableEntry(This->entries);
  while(hasNextTableEntry(iterator)){
	  deleteTableEntry( nextTableEntry(iterator) );
  }
  deleteIterTableEntry(iterator);
  deleteHashSetTableEntry(This->entries);
  ourfree(This);
}

