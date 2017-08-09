
#include "orderedge.h"

OrderEdge* allocOrderEdge(Boolean* order, OrderNode* source, OrderNode* sink){
	OrderEdge* This = (OrderEdge*) ourmalloc(sizeof(OrderEdge));
	This->source = source;
	This->sink = sink;
	This->order = order;
	return This;
}

void deleteOrderEdge(OrderEdge* This){
	ourfree(This);
}
