#include "boolean.h"

Boolean* allocBoolean(VarType t) {
	Boolean* tmp=(Boolean*) ourmalloc(sizeof (Boolean));
	tmp->btype=BOOLEANVAR;
	tmp->var.vtype=t;
	return tmp;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	Boolean* tmp=(Boolean*) ourmalloc(sizeof (Boolean));
	tmp->btype=ORDERCONST;
	tmp->order.order=order;
	tmp->order.first=first;
	tmp->order.second=second;
	return tmp;
}

void deleteBoolean(Boolean * this) {
	ourfree(this);
}
