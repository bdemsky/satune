#include "set.h"
#include <stddef.h>

Set * allocSet(VarType t, uint64_t* elements, uint num) {
	Set * tmp=(Set *)ourmalloc(sizeof(struct Set));
	tmp->type=t;
	tmp->isRange=false;
	tmp->low=0;
	tmp->high=0;
	tmp->members=allocVectorArrayInt(elements, num);
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

void freeSet(Set * set) {
	if (set->isRange)
		freeVectorInt(set->members);
	ourfree(set);
}
