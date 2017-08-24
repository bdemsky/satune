#include "orderpair.h"


OrderPair *allocOrderPair(uint64_t first, uint64_t second, Edge constraint) {
	OrderPair *pair = (OrderPair *) ourmalloc(sizeof(OrderPair));
	pair->first = first;
	pair->second = second;
	pair->constraint = constraint;
	return pair;
}

void deleteOrderPair(OrderPair *pair) {
	ourfree(pair);
}
