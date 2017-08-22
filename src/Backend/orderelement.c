#include "orderelement.h"


OrderElement *allocOrderElement(uint64_t item, Element* elem) {
	OrderElement *This = (OrderElement *) ourmalloc(sizeof(OrderElement));
	This->elem = elem;
	This->item = item;
	return This;
}

void deleteOrderElement(OrderElement *pair) {
	ourfree(pair);
}
