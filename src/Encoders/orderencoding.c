#include "orderencoding.h"

void allocOrderEncoding(OrderEncoding * This, Order *order) {
	This->type=ORDER_UNASSIGNED;
	This->order=order;
}

void deleteOrderEncoding(OrderEncoding *This) {
}
