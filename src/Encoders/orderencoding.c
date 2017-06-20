#include "orderencoding.h"

OrderEncoding * allocOrderEncoding(OrderEncodingType type, Order *order) {
	OrderEncoding *This=ourmalloc(sizeof(OrderEncoding));
	This->type=type;
	This->order=order;
	return This;
}

void deleteOrderEncoding(OrderEncoding *This) {
	ourfree(This);
}
