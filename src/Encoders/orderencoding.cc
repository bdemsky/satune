#include "orderencoding.h"
#include "orderresolver.h"

OrderEncoding::OrderEncoding(Order *_order) :
	resolver(NULL),
	type(ORDER_UNASSIGNED),
	order(_order)
{
}

OrderEncoding::~OrderEncoding() {
	if (resolver != NULL) {
		delete resolver;
	}
}
