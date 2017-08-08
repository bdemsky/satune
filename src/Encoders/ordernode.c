#include "ordernode.h"
#include "orderedge.h"

OrderNode* allocOrderNode(uint64_t id, Order* order){
	OrderNode* This = (OrderNode*) ourmalloc(sizeof(OrderNode));
	This->id = id;
	This->order = order;
	This->inEdges = allocHashSetOrderEdge(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	This->outEdges = allocHashSetOrderEdge(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	return This;
}

void deleteOrderNode(OrderNode* node){
	//NOTE: each node only responsible to delete its outgoing edges and 
	// only delete the set for incoming edges (incoming edges are going
	// to be deleted by other OrderNodes that they go out from them ...
	deleteHashSetOrderEdge(node->inEdges);
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		deleteOrderEdge(edge);
	}
	deleteIterOrderEdge(iterator);
	deleteHashSetOrderEdge(node->outEdges);
	ourfree(node);
}