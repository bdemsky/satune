#include "ordernode.h"
#include "orderedge.h"

OrderNode *allocOrderNode(uint64_t id) {
	OrderNode *This = (OrderNode *) ourmalloc(sizeof(OrderNode));
	This->id = id;
	This->inEdges = new HashSetOrderEdge();
	This->outEdges = new HashSetOrderEdge();
	This->status = NOTVISITED;
	This->sccNum = 0;
	return This;
}

void addNewIncomingEdge(OrderNode *node, OrderEdge *edge) {
	node->inEdges->add(edge);
}

void addNewOutgoingEdge(OrderNode *node, OrderEdge *edge) {
	node->outEdges->add(edge);
}

void deleteOrderNode(OrderNode *node) {
	delete node->inEdges;
	delete node->outEdges;
	ourfree(node);
}
