#include "ordergraph.h"
#include "ordernode.h"
#include "boolean.h"
#include "orderedge.h"
#include "ordergraph.h"
#include "order.h"

OrderGraph::OrderGraph(Order *_order) :
	order(_order) {
}

OrderGraph *buildOrderGraph(Order *order) {
	ASSERT(order->graph == NULL);
	OrderGraph *orderGraph = new OrderGraph(order);
	uint constrSize = order->constraints.getSize();
	for (uint j = 0; j < constrSize; j++) {
		orderGraph->addOrderConstraintToOrderGraph(order->constraints.get(j));
	}
	return orderGraph;
}

//Builds only the subgraph for the must order graph.
OrderGraph *buildMustOrderGraph(Order *order) {
	ASSERT(order->graph == NULL);
	OrderGraph *orderGraph = new OrderGraph(order);
	uint constrSize = order->constraints.getSize();
	for (uint j = 0; j < constrSize; j++) {
		orderGraph->addMustOrderConstraintToOrderGraph(order->constraints.get(j));
	}
	return orderGraph;
}

void OrderGraph::addOrderEdge(OrderNode *node1, OrderNode *node2, BooleanOrder *constr) {
	Polarity polarity = constr->polarity;
	BooleanValue mustval = constr->boolVal;
	switch (polarity) {
	case P_BOTHTRUEFALSE:
	case P_TRUE: {
		OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(node1, node2);
		if (mustval == BV_MUSTBETRUE || mustval == BV_UNSAT)
			_1to2->mustPos = true;
		_1to2->polPos = true;
		node1->addNewOutgoingEdge(_1to2);
		node2->addNewIncomingEdge(_1to2);
		if (constr->polarity == P_TRUE)
			break;
	}
	case P_FALSE: {
		if (order->type == SATC_TOTAL) {
			OrderEdge *_2to1 = getOrderEdgeFromOrderGraph( node2, node1);
			if (mustval == BV_MUSTBEFALSE || mustval == BV_UNSAT)
				_2to1->mustPos = true;
			_2to1->polPos = true;
			node2->addNewOutgoingEdge(_2to1);
			node1->addNewIncomingEdge(_2to1);
		} else {
			OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(node1, node2);
			if (mustval == BV_MUSTBEFALSE || mustval == BV_UNSAT)
				_1to2->mustNeg = true;
			_1to2->polNeg = true;
			node1->addNewOutgoingEdge(_1to2);
			node2->addNewIncomingEdge(_1to2);
		}
		break;
	}
	case P_UNDEFINED:
		//There is an unreachable order constraint if this assert fires
		//Clients can easily do this, so don't do anything.
		;
	}
}

void OrderGraph::addEdge(uint64_t first, uint64_t second) {
	OrderNode *node1 = getOrderNodeFromOrderGraph(first);
	OrderNode *node2 = getOrderNodeFromOrderGraph(second);
	OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(node1, node2);
	_1to2->polPos = true;
	_1to2->mustPos = true;
	node1->addNewOutgoingEdge(_1to2);
	node2->addNewIncomingEdge(_1to2);
}

void OrderGraph::addMustOrderEdge(OrderNode *node1, OrderNode *node2, BooleanOrder *constr) {
	BooleanValue mustval = constr->boolVal;
	switch (mustval) {
	case BV_UNSAT:
	case BV_MUSTBETRUE: {
		OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(node1, node2);
		_1to2->mustPos = true;
		_1to2->polPos = true;
		node1->addNewOutgoingEdge(_1to2);
		node2->addNewIncomingEdge(_1to2);
		if (constr->boolVal == BV_MUSTBETRUE)
			break;
	}
	case BV_MUSTBEFALSE: {
		if (order->type == SATC_TOTAL) {
			OrderEdge *_2to1 = getOrderEdgeFromOrderGraph(node2, node1);
			_2to1->mustPos = true;
			_2to1->polPos = true;
			node2->addNewOutgoingEdge(_2to1);
			node1->addNewIncomingEdge(_2to1);
		} else {
			OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(node1, node2);
			_1to2->mustNeg = true;
			_1to2->polNeg = true;
			node1->addNewOutgoingEdge(_1to2);
			node2->addNewIncomingEdge(_1to2);
		}
		break;
	}
	case BV_UNDEFINED:
		//Do Nothing
		break;
	}
}

OrderNode *OrderGraph::getOrderNodeFromOrderGraph(uint64_t id) {
	OrderNode *node = new OrderNode(id);
	OrderNode *tmp = (OrderNode *)nodes.get(node);
	if ( tmp != NULL) {
		delete node;
		node = tmp;
	} else {
		nodes.add(node);
	}
	return node;
}

OrderNode *OrderGraph::lookupOrderNodeFromOrderGraph(uint64_t id) {
	OrderNodeKey node(id);
	OrderNode *tmp = (OrderNode *)nodes.get(&node);
	return tmp;
}

OrderEdge *OrderGraph::getOrderEdgeFromOrderGraph(OrderNode *begin, OrderNode *end) {
	OrderEdge *edge = new OrderEdge(begin, end);
	OrderEdge *tmp = edges.get(edge);
	if ( tmp != NULL ) {
		delete edge;
		edge = tmp;
	} else {
		edges.add(edge);
	}
	return edge;
}

OrderEdge *OrderGraph::lookupOrderEdgeFromOrderGraph(OrderNode *begin, OrderNode *end) {
	OrderEdge edge(begin, end);
	OrderEdge *tmp = edges.get(&edge);
	return tmp;
}

OrderEdge *OrderGraph::getInverseOrderEdge(OrderEdge *edge) {
	OrderEdge inverseedge(edge->sink, edge->source);
	OrderEdge *tmp = edges.get(&inverseedge);
	return tmp;
}

void OrderGraph::addOrderConstraintToOrderGraph(BooleanOrder *bOrder) {
	OrderNode *from = getOrderNodeFromOrderGraph(bOrder->first);
	OrderNode *to = getOrderNodeFromOrderGraph(bOrder->second);
	addOrderEdge(from, to, bOrder);
}

void OrderGraph::addMustOrderConstraintToOrderGraph(BooleanOrder *bOrder) {
	OrderNode *from = getOrderNodeFromOrderGraph(bOrder->first);
	OrderNode *to = getOrderNodeFromOrderGraph(bOrder->second);
	addMustOrderEdge(from, to, bOrder);
}

OrderGraph::~OrderGraph() {
	nodes.resetAndDelete();
	edges.resetAndDelete();
}

bool OrderGraph::isTherePath(OrderNode *source, OrderNode *destination) {
	HashsetOrderNode visited;
	visited.add(source);
	SetIteratorOrderEdge *iterator = source->outEdges.iterator();
	bool found = false;
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (edge->polPos) {
			OrderNode *node = edge->sink;
			if (!visited.contains(node)) {
				if ( node == destination ) {
					found = true;
					break;
				}
				visited.add(node);
				found = isTherePathVisit(visited, node, destination);
				if (found) {
					break;
				}
			}
		}
	}
	delete iterator;
	return found;
}

bool OrderGraph::isTherePathVisit(HashsetOrderNode &visited, OrderNode *current, OrderNode *destination) {
	SetIteratorOrderEdge *iterator = current->outEdges.iterator();
	bool found = false;
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (edge->polPos) {
			OrderNode *node = edge->sink;
			if (node == destination) {
				found = true;
				break;
			}
			visited.add(node);
			if (isTherePathVisit(visited, node, destination)) {
				found = true;
				break;
			}
		}
	}
	delete iterator;
	return found;
}

void OrderGraph::DFS(Vector<OrderNode *> *finishNodes) {
	SetIteratorOrderNode *iterator = getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = (OrderNode *)iterator->next();
		if (node->status == NOTVISITED && !node->removed) {
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, false, 0);
			node->status = FINISHED;
			finishNodes->push(node);
		}
	}
	delete iterator;
}

void OrderGraph::DFSMust(Vector<OrderNode *> *finishNodes) {
	SetIteratorOrderNode *iterator = getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = (OrderNode *)iterator->next();
		if (node->status == NOTVISITED && !node->removed) {
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, true, 0);
			node->status = FINISHED;
			finishNodes->push(node);
		}
	}
	delete iterator;
}

void OrderGraph::DFSReverse(Vector<OrderNode *> *finishNodes) {
	uint size = finishNodes->getSize();
	uint sccNum = 1;
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes->get(i);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, NULL, true, false, sccNum);
			node->sccNum = sccNum;
			node->status = FINISHED;
			sccNum++;
		}
	}
}

void OrderGraph::DFSNodeVisit(OrderNode *node, Vector<OrderNode *> *finishNodes, bool isReverse, bool mustvisit, uint sccNum) {
	SetIteratorOrderEdge *iterator = isReverse ? node->inEdges.iterator() : node->outEdges.iterator();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (mustvisit) {
			if (!edge->mustPos)
				continue;
		} else
		if (!edge->polPos && !edge->pseudoPos)		//Ignore edges that do not have positive polarity
			continue;

		OrderNode *child = isReverse ? edge->source : edge->sink;
		if (child->status == NOTVISITED) {
			child->status = VISITED;
			DFSNodeVisit(child, finishNodes, isReverse, mustvisit, sccNum);
			child->status = FINISHED;
			if (finishNodes != NULL) {
				finishNodes->push(child);
			}
			if (isReverse)
				child->sccNum = sccNum;
		}
	}
	delete iterator;
}

void OrderGraph::resetNodeInfoStatusSCC() {
	SetIteratorOrderNode *iterator = getNodes();
	while (iterator->hasNext()) {
		((OrderNode *)iterator->next())->status = NOTVISITED;
	}
	delete iterator;
}

void OrderGraph::computeStronglyConnectedComponentGraph() {
	Vector<OrderNode *> finishNodes;
	DFS(&finishNodes);
	resetNodeInfoStatusSCC();
	DFSReverse(&finishNodes);
	resetNodeInfoStatusSCC();
}

/** This function computes a source set for every nodes, the set of
    nodes that can reach that node via pospolarity edges.  It then
    looks for negative polarity edges from nodes in the the source set
    to determine whether we need to generate pseudoPos edges. */

void OrderGraph::completePartialOrderGraph() {
	Vector<OrderNode *> finishNodes;
	DFS(&finishNodes);
	resetNodeInfoStatusSCC();
	HashtableNodeToNodeSet *table = new HashtableNodeToNodeSet(128, 0.25);

	Vector<OrderNode *> sccNodes;

	uint size = finishNodes.getSize();
	uint sccNum = 1;
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes.get(i);
		HashsetOrderNode *sources = new HashsetOrderNode(4, 0.25);
		table->put(node, sources);

		if (node->status == NOTVISITED) {
			//Need to do reverse traversal here...
			node->status = VISITED;
			DFSNodeVisit(node, &sccNodes, true, false, sccNum);
			node->status = FINISHED;
			node->sccNum = sccNum;
			sccNum++;
			sccNodes.push(node);

			//Compute in set for entire SCC
			uint rSize = sccNodes.getSize();
			for (uint j = 0; j < rSize; j++) {
				OrderNode *rnode = sccNodes.get(j);
				//Compute source sets
				SetIteratorOrderEdge *iterator = rnode->inEdges.iterator();
				while (iterator->hasNext()) {
					OrderEdge *edge = iterator->next();
					OrderNode *parent = edge->source;
					if (edge->polPos) {
						sources->add(parent);
						HashsetOrderNode *parent_srcs = (HashsetOrderNode *)table->get(parent);
						sources->addAll(parent_srcs);
					}
				}
				delete iterator;
			}
			for (uint j = 0; j < rSize; j++) {
				//Copy in set of entire SCC
				OrderNode *rnode = sccNodes.get(j);
				HashsetOrderNode *set = (j == 0) ? sources : sources->copy();
				table->put(rnode, set);

				//Use source sets to compute pseudoPos edges
				SetIteratorOrderEdge *iterator = node->inEdges.iterator();
				while (iterator->hasNext()) {
					OrderEdge *edge = iterator->next();
					OrderNode *parent = edge->source;
					ASSERT(parent != rnode);
					if (edge->polNeg && parent->sccNum != rnode->sccNum &&
							sources->contains(parent)) {
						OrderEdge *newedge = getOrderEdgeFromOrderGraph(rnode, parent);
						newedge->pseudoPos = true;
					}
				}
				delete iterator;
			}

			sccNodes.clear();
		}
	}

	table->resetAndDeleteVals();
	delete table;
	resetNodeInfoStatusSCC();
}
