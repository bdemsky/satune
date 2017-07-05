#include "orderpair.h"


OrderPair* allocOrderPair(uint64_t first, uint64_t second){
	OrderPair* pair = (OrderPair*) ourmalloc(sizeof(OrderPair));
	pair->first = first;
	pair->second = second;
	return pair;
}
void deleteOrderPair(OrderPair* pair){
	ourfree(pair);
}