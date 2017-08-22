#include "ordergraph.h"
#include "ordernode.h"
#include "boolean.h"
#include "orderedge.h"
#include "ordergraph.h"
#include "order.h"

OrderGraph *allocOrderGraph(Order *order) {
	OrderGraph *This = (OrderGraph *) ourmalloc(sizeof(OrderGraph));
	This->nodes = allocHashSetOrderNode(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	This->edges = allocHashSetOrderEdge(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	This->order = order;
	return This;
}

OrderGraph *buildOrderGraph(Order *order) {
	OrderGraph *orderGraph = allocOrderGraph(order);
	uint constrSize = getSizeVectorBooleanOrder(&order->constraints);
	for (uint j = 0; j < constrSize; j++) {
		addOrderConstraintToOrderGraph(orderGraph, getVectorBooleanOrder(&order->constraints, j));
	}
	return orderGraph;
}

//Builds only the subgraph for the must order graph.
OrderGraph *buildMustOrderGraph(Order *order) {
	OrderGraph *orderGraph = allocOrderGraph(order);
	uint constrSize = getSizeVectorBooleanOrder(&order->constraints);
	for (uint j = 0; j < constrSize; j++) {
		addMustOrderConstraintToOrderGraph(orderGraph, getVectorBooleanOrder(&order->constraints, j));
	}
	return orderGraph;
}

void addOrderEdge(OrderGraph *graph, OrderNode *node1, OrderNode *node2, BooleanOrder *constr) {
	Polarity polarity = constr->base.polarity;
	BooleanValue mustval = constr->base.boolVal;
	Order *order = graph->order;
	switch (polarity) {
	case P_BOTHTRUEFALSE:
	case P_TRUE: {
		OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
		if (mustval == BV_MUSTBETRUE || mustval == BV_UNSAT)
			_1to2->mustPos = true;
		_1to2->polPos = true;
		addNewOutgoingEdge(node1, _1to2);
		addNewIncomingEdge(node2, _1to2);
		if (constr->base.polarity == P_TRUE)
			break;
	}
	case P_FALSE: {
		if (order->type == TOTAL) {
			OrderEdge *_2to1 = getOrderEdgeFromOrderGraph(graph, node2, node1);
			if (mustval == BV_MUSTBEFALSE || mustval == BV_UNSAT)
				_2to1->mustPos = true;
			_2to1->polPos = true;
			addNewOutgoingEdge(node2, _2to1);
			addNewIncomingEdge(node1, _2to1);
		} else {
			OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
			if (mustval == BV_MUSTBEFALSE || mustval == BV_UNSAT)
				_1to2->mustNeg = true;
			_1to2->polNeg = true;
			addNewOutgoingEdge(node1, _1to2);
			addNewIncomingEdge(node2, _1to2);
		}
		break;
	}
	case P_UNDEFINED:
		//There is an unreachable order constraint if this assert fires
		ASSERT(0);
	}
}

void addMustOrderEdge(OrderGraph *graph, OrderNode *node1, OrderNode *node2, BooleanOrder *constr) {
	BooleanValue mustval = constr->base.boolVal;
	Order *order = graph->order;
	switch (mustval) {
	case BV_UNSAT:
	case BV_MUSTBETRUE: {
		OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
		_1to2->mustPos = true;
		_1to2->polPos = true;
		addNewOutgoingEdge(node1, _1to2);
		addNewIncomingEdge(node2, _1to2);
		if (constr->base.polarity == BV_MUSTBETRUE)
			break;
	}
	case BV_MUSTBEFALSE: {
		if (order->type == TOTAL) {
			OrderEdge *_2to1 = getOrderEdgeFromOrderGraph(graph, node2, node1);
			_2to1->mustPos = true;
			_2to1->polPos = true;
			addNewOutgoingEdge(node2, _2to1);
			addNewIncomingEdge(node1, _2to1);
		} else {
			OrderEdge *_1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
			_1to2->mustNeg = true;
			_1to2->polNeg = true;
			addNewOutgoingEdge(node1, _1to2);
			addNewIncomingEdge(node2, _1to2);
		}
		break;
	}
	case BV_UNDEFINED:
		//Do Nothing
		break;
	}
}

OrderNode *getOrderNodeFromOrderGraph(OrderGraph *graph, uint64_t id) {
	OrderNode *node = allocOrderNode(id);
	OrderNode *tmp = getHashSetOrderNode(graph->nodes, node);
	if ( tmp != NULL) {
		deleteOrderNode(node);
		node = tmp;
	} else {
		addHashSetOrderNode(graph->nodes, node);
	}
	return node;
}

OrderNode *lookupOrderNodeFromOrderGraph(OrderGraph *graph, uint64_t id) {
	OrderNode node = {id, NULL, NULL, 0, 0};
	OrderNode *tmp = getHashSetOrderNode(graph->nodes, &node);
	return tmp;
}

OrderEdge *getOrderEdgeFromOrderGraph(OrderGraph *graph, OrderNode *begin, OrderNode *end) {
	OrderEdge *edge = allocOrderEdge(begin, end);
	OrderEdge *tmp = getHashSetOrderEdge(graph->edges, edge);
	if ( tmp != NULL ) {
		deleteOrderEdge(edge);
		edge = tmp;
	} else {
		addHashSetOrderEdge(graph->edges, edge);
	}
	return edge;
}

OrderEdge *lookupOrderEdgeFromOrderGraph(OrderGraph *graph, OrderNode *begin, OrderNode *end) {
	OrderEdge edge = {begin, end, 0, 0, 0, 0, 0};
	OrderEdge *tmp = getHashSetOrderEdge(graph->edges, &edge);
	return tmp;
}

OrderEdge *getInverseOrderEdge(OrderGraph *graph, OrderEdge *edge) {
	OrderEdge inverseedge = {edge->sink, edge->source, false, false, false, false, false};
	OrderEdge *tmp = getHashSetOrderEdge(graph->edges, &inverseedge);
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
	HSIteratorOrderNode *iterator = iteratorOrderNode(graph->nodes);
	while (hasNextOrderNode(iterator)) {
		OrderNode *node = nextOrderNode(iterator);
		deleteOrderNode(node);
	}
	deleteIterOrderNode(iterator);

	HSIteratorOrderEdge *eiterator = iteratorOrderEdge(graph->edges);
	while (hasNextOrderEdge(eiterator)) {
		OrderEdge *edge = nextOrderEdge(eiterator);
		deleteOrderEdge(edge);
	}
	deleteIterOrderEdge(eiterator);
	ourfree(graph);
}
