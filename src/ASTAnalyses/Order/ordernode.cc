#include "ordernode.h"
#include "orderedge.h"

OrderNode::OrderNode(uint64_t _id) :
	OrderNodeKey(_id),
	status(NOTVISITED),
	removed(false),
	sccNum(0),
	inEdges(),
	outEdges() {
}

void OrderNode::addNewIncomingEdge(OrderEdge *edge) {
	inEdges.add(edge);
}

void OrderNode::addNewOutgoingEdge(OrderEdge *edge) {
	outEdges.add(edge);
}
