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

void addNewIncomingEdge(OrderNode* node, OrderEdge* edge){
	ASSERT(!containsHashSetOrderEdge(node->inEdges, edge)); // Only for testing ... Should be removed after testing
	addHashSetOrderEdge(node->inEdges, edge);
}

void addNewOutgoingEdge(OrderNode* node, OrderEdge* edge){
	ASSERT(!containsHashSetOrderEdge(node->outEdges, edge));
	addHashSetOrderEdge(node->outEdges, edge);
}

void deleteOrderNode(OrderNode* node){
	deleteHashSetOrderEdge(node->inEdges);
	deleteHashSetOrderEdge(node->outEdges);
	ourfree(node);
}