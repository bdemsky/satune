#include "orderedge.h"
#include "ordergraph.h"

OrderEdge *allocOrderEdge(OrderNode *source, OrderNode *sink) {
	OrderEdge *This = (OrderEdge *) ourmalloc(sizeof(OrderEdge));
	This->source = source;
	This->sink = sink;
	This->polPos = false;
	This->polNeg = false;
	This->mustPos = false;
	This->mustNeg = false;
	This->pseudoPos = false;
	return This;
}

void deleteOrderEdge(OrderEdge *This) {
	ourfree(This);
}

