#include "set.h"
#include <stddef.h>

Set * allocSet(VarType t, uint64_t* elements, uint num) {
	Set * tmp=(Set *)ourmalloc(sizeof(Set));
	tmp->type=t;
	tmp->isRange=false;
	tmp->low=0;
	tmp->high=0;
	tmp->members=allocVectorArrayInt(num, elements);
	return tmp;
}

Set * allocSetRange(VarType t, uint64_t lowrange, uint64_t highrange) {
	Set * tmp=(Set *)ourmalloc(sizeof(Set));
	tmp->type=t;
	tmp->isRange=true;
	tmp->low=lowrange;
	tmp->high=highrange;
	tmp->members=NULL;
	return tmp;
}

bool existsInSet(Set* set, uint64_t element){
	if(set->isRange){
		return element >= set->low && element <= set->high;
	}else {
		uint size = getSizeVectorInt(set->members);
		for(uint i=0; i< size; i++){
			if(element == getVectorInt(set->members, i))
				return true;
		}
		return false;
	}
}

uint getSetSize(Set* set){
	if(set->isRange){
		return set->high- set->low+1;
	}else{
		return getSizeVectorInt(set->members);
	}
}

void deleteSet(Set * set) {
	if (!set->isRange)
		deleteVectorInt(set->members);
	ourfree(set);
}
