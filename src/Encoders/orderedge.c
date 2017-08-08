
#include "orderedge.h"

OrderEdge* allocOrderEdge(Boolean* order, OrderNode* begin, OrderNode* end){
	OrderEdge* This = (OrderEdge*) ourmalloc(sizeof(OrderEdge));
	This->source = begin;
	This->sink = end;
	This->order = order;
	return This;
}

void deleteOrderEdge(OrderEdge* This){
	ourfree(This);
}
