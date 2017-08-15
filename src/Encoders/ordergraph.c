#include "ordergraph.h"
#include "ordernode.h"
#include "boolean.h"
#include "orderedge.h"
#include "ordergraph.h"
#include "order.h"

OrderGraph* allocOrderGraph(Order *order) {
	OrderGraph* This = (OrderGraph*) ourmalloc(sizeof(OrderGraph));
	This->nodes = allocHashSetOrderNode(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	This->order = order;
	initDefVectorOrderNode(&This->scc);
	return This;
}

void addOrderEdge(OrderGraph* graph, OrderNode* node1, OrderNode* node2, Boolean* constr) {
	Polarity polarity=constr->polarity;
	BooleanValue mustval=constr->boolVal;
	Order* order=graph->order;
	switch(polarity) {
	case P_BOTHTRUEFALSE:
	case P_TRUE:{
		OrderEdge* _1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
		if (mustval==BV_MUSTBETRUE || mustval == BV_UNSAT)
			_1to2->mustPos = true;
		_1to2->polPos = true;
		addNewOutgoingEdge(node1, _1to2);
		addNewIncomingEdge(node2, _1to2);
		if(constr->polarity == P_TRUE)
			break;
	}
	case P_FALSE:{
		if (order->type==TOTAL) {
			OrderEdge* _2to1 = getOrderEdgeFromOrderGraph(graph, node2, node1);
			if (mustval==BV_MUSTBEFALSE || mustval == BV_UNSAT)
				_2to1->mustPos = true;
			_2to1->polPos = true;
			addNewOutgoingEdge(node2, _2to1);
			addNewIncomingEdge(node1, _2to1);
		} else {
			OrderEdge* _1to2 = getOrderEdgeFromOrderGraph(graph, node1, node2);
			if (mustval==BV_MUSTBEFALSE || mustval == BV_UNSAT)
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

OrderNode* getOrderNodeFromOrderGraph(OrderGraph* graph, uint64_t id) {
	OrderNode* node = allocOrderNode(id);
	OrderNode* tmp = getHashSetOrderNode(graph->nodes, node);
	if( tmp != NULL){
		deleteOrderNode(node);
		node = tmp;
	} else {
		addHashSetOrderNode(graph->nodes, node);
	}
	return node;
}

OrderEdge* getOrderEdgeFromOrderGraph(OrderGraph* graph, OrderNode* begin, OrderNode* end) {
	OrderEdge* edge = allocOrderEdge(begin, end);
	OrderEdge* tmp = getHashSetOrderEdge(graph->edges, edge);
	if ( tmp!= NULL ) {
		deleteOrderEdge(edge);
		edge = tmp;
	} else {
		addHashSetOrderEdge(graph->edges, edge);
	}
	return edge;
}

OrderEdge* getInverseOrderEdge(OrderGraph* graph, OrderEdge *edge) {
	OrderEdge inverseedge={edge->sink, edge->source, false, false, false, false, false};
	OrderEdge * tmp=getHashSetOrderEdge(graph->edges, &inverseedge);
	return tmp;
}

void addOrderConstraintToOrderGraph(OrderGraph* graph, Boolean* constr) {
	BooleanOrder* bOrder = (BooleanOrder*) constr;
	OrderNode* from = getOrderNodeFromOrderGraph(graph, bOrder->first);
	OrderNode* to = getOrderNodeFromOrderGraph(graph, bOrder->second);
	addOrderEdge(graph, from, to, constr);
}

void deleteOrderGraph(OrderGraph* graph){
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		deleteOrderNode(node);
	}
	deleteIterOrderNode(iterator);
	
	HSIteratorOrderEdge* eiterator = iteratorOrderEdge(graph->edges);
	while(hasNextOrderEdge(eiterator)){
		OrderEdge* edge = nextOrderEdge(eiterator);
		deleteOrderEdge(edge);
	}
	deleteIterOrderEdge(eiterator);
	ourfree(graph);
}
