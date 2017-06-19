#include "orderencoding.h"

OrderEncoding * allocOrderEncoding() {
	OrderEncoding *This=ourmalloc(sizeof(OrderEncoding));
	return This;
}

void deleteOrderEncoding(OrderEncoding *This) {
	ourfree(This);
}
