#include "ordergraph.h"
#include "ordernode.h"
#include "boolean.h"
#include "orderedge.h"
#include "ordergraph.h"
#include "order.h"

OrderGraph::OrderGraph(Order *_order) :
	nodes(new HashsetOrderNode()),
	edges(new HashsetOrderEdge()),
	order(_order) {
}

OrderGraph *buildOrderGraph(Order *order) {
	OrderGraph *orderGraph = new OrderGraph(order);
	uint constrSize = order->constraints.getSize();
	for (uint j = 0; j < constrSize; j++) {
		orderGraph->addOrderConstraintToOrderGraph(order->constraints.get(j));
	}
	return orderGraph;
}

//Builds only the subgraph for the must order graph.
OrderGraph *buildMustOrderGraph(Order *order) {
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
		ASSERT(0);
	}
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
	OrderNode *tmp = nodes->get(node);
	if ( tmp != NULL) {
		delete node;
		node = tmp;
	} else {
		nodes->add(node);
	}
	return node;
}

OrderNode *OrderGraph::lookupOrderNodeFromOrderGraph(uint64_t id) {
	OrderNode node(id);
	OrderNode *tmp = nodes->get(&node);
	return tmp;
}

OrderEdge *OrderGraph::getOrderEdgeFromOrderGraph(OrderNode *begin, OrderNode *end) {
	OrderEdge *edge = new OrderEdge(begin, end);
	OrderEdge *tmp = edges->get(edge);
	if ( tmp != NULL ) {
		delete edge;
		edge = tmp;
	} else {
		edges->add(edge);
	}
	return edge;
}

OrderEdge *OrderGraph::lookupOrderEdgeFromOrderGraph(OrderNode *begin, OrderNode *end) {
	OrderEdge edge(begin, end);
	OrderEdge *tmp = edges->get(&edge);
	return tmp;
}

OrderEdge *OrderGraph::getInverseOrderEdge(OrderEdge *edge) {
	OrderEdge inverseedge(edge->sink, edge->source);
	OrderEdge *tmp = edges->get(&inverseedge);
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
	SetIteratorOrderNode *iterator = nodes->iterator();
	while (iterator->hasNext()) {
		OrderNode *node = iterator->next();
		delete node;
	}
	delete iterator;

	SetIteratorOrderEdge *eiterator = edges->iterator();
	while (eiterator->hasNext()) {
		OrderEdge *edge = eiterator->next();
		delete edge;
	}
	delete eiterator;
	delete nodes;
	delete edges;
}
