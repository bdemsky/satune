#include "tableentry.h"
#include <string.h>

TableEntry* allocTableEntry(uint64_t* inputs, uint inputSize, uint64_t result){
	TableEntry* te = (TableEntry*) ourmalloc(sizeof(TableEntry)+inputSize*sizeof(uint64_t));
	te->output=result;
	memcpy(te->inputs, inputs, inputSize * sizeof(uint64_t));
	return te;
}

void deleteTableEntry(TableEntry* tableEntry){
	ourfree(tableEntry);
}
