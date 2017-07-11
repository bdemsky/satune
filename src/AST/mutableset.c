#include "mutableset.h"

MutableSet * allocMutableSet(VarType t) {
	MutableSet * This=(MutableSet *)ourmalloc(sizeof(MutableSet));
	This->type=t;
	This->isRange=false;
	This->low=0;
	This->high=0;
	This->members=allocDefVectorInt();
	return This;
}

void addElementMSet(MutableSet * set, uint64_t element) {
	pushVectorInt(set->members, element);
}
