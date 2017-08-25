#include "orderencoding.h"

OrderEncoding::OrderEncoding(Order *_order) :
	type(ORDER_UNASSIGNED),
	order(_order) {
}
