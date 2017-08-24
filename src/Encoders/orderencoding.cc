#include "orderencoding.h"

void initOrderEncoding(OrderEncoding *This, Order *order) {
	This->type = ORDER_UNASSIGNED;
	This->order = order;
}

void deleteOrderEncoding(OrderEncoding *This) {
}
