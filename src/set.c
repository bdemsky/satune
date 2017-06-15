#include "set.h"
#include <stddef.h>

Set * allocSet(VarType t, uint64_t* elements, uint num) {
	Set * tmp=(Set *)ourmalloc(sizeof(struct Set));
	tmp->type=t;
	tmp->isRange=false;
	tmp->low=0;
	tmp->high=0;
	tmp->members=allocVectorArrayInt(num, elements);
	return tmp;
}

Set * allocSetRange(VarType t, uint64_t lowrange, uint64_t highrange) {
	Set * tmp=(Set *)ourmalloc(sizeof(struct Set));
	tmp->type=t;
	tmp->isRange=true;
	tmp->low=lowrange;
	tmp->high=highrange;
	tmp->members=NULL;
	return tmp;
}

void deleteSet(Set * set) {
	if (set->isRange)
		deleteVectorInt(set->members);
	ourfree(set);
}
