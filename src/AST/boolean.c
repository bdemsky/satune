#include "boolean.h"

Boolean* allocBoolean(VarType t) {
	BooleanVar* tmp=(BooleanVar *) ourmalloc(sizeof (BooleanVar));
	GETBOOLEANTYPE(tmp)=BOOLEANVAR;
	tmp->vtype=t;
	tmp->var=NULL;
	return & tmp->base;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	BooleanOrder* tmp=(BooleanOrder *) ourmalloc(sizeof (BooleanOrder));
	GETBOOLEANTYPE(tmp)=ORDERCONST;
	tmp->order=order;
	tmp->first=first;
	tmp->second=second;
	return & tmp -> base;
}

void deleteBoolean(Boolean * This) {
	ourfree(This);
}
