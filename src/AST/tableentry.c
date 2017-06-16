#include "tableentry.h"

TableEntry* allocTableEntry(uint64_t* inputs, uint inputSize, uint64_t result){
    TableEntry* te = (TableEntry*) ourmalloc(sizeof(TableEntry)+inputSize*sizeof(uint64_t));
    te->output=result;
    for(int i=0; i<inputSize; i++){
	te->inputs[i]=inputs[i];
    }
    return te;
}