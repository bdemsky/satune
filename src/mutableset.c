#include "mutableset.h"

MutableSet * allocMutableSet(VarType t) {
	MutableSet * tmp=(MutableSet *)ourmalloc(sizeof(MutableSet));
	tmp->type=t;
	tmp->isRange=false;
	tmp->low=0;
	tmp->high=0;
	tmp->members=allocDefVectorInt();
	return tmp;
}

void addElementMSet(MutableSet * set, uint64_t element) {
	pushVectorInt(set->members, element);
}
