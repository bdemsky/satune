#include "ordergraph.h"
#include "ordernode.h"
#include "boolean.h"
#include "orderedge.h"
#include "ordergraph.h"
#include "order.h"

OrderGraph *allocOrderGraph(Order *order) {
	OrderGraph *This = (OrderGraph *) ourmalloc(sizeof(OrderGraph));
	This->nodes = new HashSetOrderNode();
	This->edges = new HashSetOrderEdge();
	This->order = order;
	return This;
}

OrderGraph *buildOrderGraph(Order *order) {
	OrderGraph *orderGraph = allocOrderGraph(order);
	uint constrSize = order->constraints.getSize();
	for (uint j = 0; j < constrSize; j++) {
		addOrderConstraintToOrderGraph(orderGraph, order->constraints.get(j));
	}
	return orderGraph;
}

//Builds only the subgraph for the must order graph.
OrderGraph *buildMustOrderGraph(Order *order) {
	OrderGraph *orderGraph = allocOrderGraph(order);
	uint constrSize = order->constraints.getSize();
	for (uint j = 0; j < constrSize; j++) {
		addMustOrderConstraintToOrderGraph(orderGraph, order->constraints.get(j));
	}
	return orderGraph;
}

void addOrderEdge(OrderGraph *graph, OrderNode *node1, OrderNode *node2, BooleanOrder *constr) {
	Polarity polarity = constr->polarity;
	BooleanValue mustval = constr->boolVal;
	Order *order = graph->order;
	switch (polarity) {
	case P_BOTHTRUEFALSE:
	case P_TRUE: {
		OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
		if (mustval == BV_MUSTBETRUE || mustval == BV_UNSAT)
			_1to2->mustPos = true;
		_1to2->polPos = true;
		node1->addNewOutgoingEdge(_1to2);
		node2->addNewIncomingEdge(_1to2);
		if (constr->polarity == P_TRUE)
			break;
	}
	case P_FALSE: {
		if (order->type == TOTAL) {
			OrderEdge *_2to1 = getOrderEdgeFromOrderGraph(graph, node2, node1);
			if (mustval == BV_MUSTBEFALSE || mustval == BV_UNSAT)
				_2to1->mustPos = true;
			_2to1->polPos = true;
			node2->addNewOutgoingEdge(_2to1);
			node1->addNewIncomingEdge(_2to1);
		} else {
			OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
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

void addMustOrderEdge(OrderGraph *graph, OrderNode *node1, OrderNode *node2, BooleanOrder *constr) {
	BooleanValue mustval = constr->boolVal;
	Order *order = graph->order;
	switch (mustval) {
	case BV_UNSAT:
	case BV_MUSTBETRUE: {
		OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
		_1to2->mustPos = true;
		_1to2->polPos = true;
		node1->addNewOutgoingEdge(_1to2);
		node2->addNewIncomingEdge(_1to2);
		if (constr->boolVal == BV_MUSTBETRUE)
			break;
	}
	case BV_MUSTBEFALSE: {
		if (order->type == TOTAL) {
			OrderEdge *_2to1 = getOrderEdgeFromOrderGraph(graph, node2, node1);
			_2to1->mustPos = true;
			_2to1->polPos = true;
			node2->addNewOutgoingEdge(_2to1);
			node1->addNewIncomingEdge(_2to1);
		} else {
			OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
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

OrderNode *getOrderNodeFromOrderGraph(OrderGraph *graph, uint64_t id) {
	OrderNode *node = new OrderNode(id);
	OrderNode *tmp = graph->nodes->get(node);
	if ( tmp != NULL) {
		delete node;
		node = tmp;
	} else {
		graph->nodes->add(node);
	}
	return node;
}

OrderNode *lookupOrderNodeFromOrderGraph(OrderGraph *graph, uint64_t id) {
	OrderNode node(id);
	OrderNode *tmp = graph->nodes->get(&node);
	return tmp;
}

OrderEdge *getOrderEdgeFromOrderGraph(OrderGraph *graph, OrderNode *begin, OrderNode *end) {
	OrderEdge *edge = new OrderEdge(begin, end);
	OrderEdge *tmp = graph->edges->get(edge);
	if ( tmp != NULL ) {
		delete edge;
		edge = tmp;
	} else {
		graph->edges->add(edge);
	}
	return edge;
}

OrderEdge *lookupOrderEdgeFromOrderGraph(OrderGraph *graph, OrderNode *begin, OrderNode *end) {
	OrderEdge edge(begin, end);
	OrderEdge *tmp = graph->edges->get(&edge);
	return tmp;
}

OrderEdge *getInverseOrderEdge(OrderGraph *graph, OrderEdge *edge) {
	OrderEdge inverseedge(edge->sink, edge->source);
	OrderEdge *tmp = graph->edges->get(&inverseedge);
	return tmp;
}

void addOrderConstraintToOrderGraph(OrderGraph *graph, BooleanOrder *bOrder) {
	OrderNode *from = getOrderNodeFromOrderGraph(graph, bOrder->first);
	OrderNode *to = getOrderNodeFromOrderGraph(graph, bOrder->second);
	addOrderEdge(graph, from, to, bOrder);
}

void addMustOrderConstraintToOrderGraph(OrderGraph *graph, BooleanOrder *bOrder) {
	OrderNode *from = getOrderNodeFromOrderGraph(graph, bOrder->first);
	OrderNode *to = getOrderNodeFromOrderGraph(graph, bOrder->second);
	addMustOrderEdge(graph, from, to, bOrder);
}

void deleteOrderGraph(OrderGraph *graph) {
	HSIteratorOrderNode *iterator = graph->nodes->iterator();
	while (iterator->hasNext()) {
		OrderNode *node = iterator->next();
		delete node;
	}
	delete iterator;

	HSIteratorOrderEdge *eiterator = graph->edges->iterator();
	while (eiterator->hasNext()) {
		OrderEdge *edge = eiterator->next();
		delete edge;
	}
	delete eiterator;
	delete graph->nodes;
	delete graph->edges;
	ourfree(graph);
}
