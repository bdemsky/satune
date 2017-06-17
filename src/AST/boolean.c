#include "boolean.h"

Boolean* allocBoolean(VarType t) {
	BooleanVar* tmp=(BooleanVar *) ourmalloc(sizeof (BooleanVar));
	tmp->base.btype=BOOLEANVAR;
	tmp->vtype=t;
	return & tmp->base;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	BooleanOrder* tmp=(BooleanOrder *) ourmalloc(sizeof (BooleanOrder));
	tmp->base.btype=ORDERCONST;
	tmp->order=order;
	tmp->first=first;
	tmp->second=second;
	return & tmp -> base;
}

void deleteBoolean(Boolean * this) {
	ourfree(this);
}
