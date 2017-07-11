#include "set.h"
#include <stddef.h>

Set * allocSet(VarType t, uint64_t* elements, uint num) {
	Set * This=(Set *)ourmalloc(sizeof(Set));
	This->type=t;
	This->isRange=false;
	This->low=0;
	This->high=0;
	This->members=allocVectorArrayInt(num, elements);
	return This;
}

Set * allocSetRange(VarType t, uint64_t lowrange, uint64_t highrange) {
	Set * This=(Set *)ourmalloc(sizeof(Set));
	This->type=t;
	This->isRange=true;
	This->low=lowrange;
	This->high=highrange;
	This->members=NULL;
	return This;
}

bool existsInSet(Set* This, uint64_t element){
	if(This->isRange){
		return element >= This->low && element <= This->high;
	}else {
		uint size = getSizeVectorInt(This->members);
		for(uint i=0; i< size; i++){
			if(element == getVectorInt(This->members, i))
				return true;
		}
		return false;
	}
}

uint getSetSize(Set* This){
	if(This->isRange){
		return This->high- This->low+1;
	}else{
		return getSizeVectorInt(This->members);
	}
}

void deleteSet(Set * This) {
	if (!This->isRange)
		deleteVectorInt(This->members);
	ourfree(This);
}
