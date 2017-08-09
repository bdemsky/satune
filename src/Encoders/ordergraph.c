#include "ordergraph.h"
#include "ordernode.h"
#include "boolean.h"
#include "orderedge.h"

OrderGraph* allocOrderGraph(){
	OrderGraph* This = (OrderGraph*) ourmalloc(sizeof(OrderGraph));
	This->nodes = allocHashSetOrderNode(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	return This;
}

void addOrderEdge(OrderGraph* graph, OrderNode* node1, OrderNode* node2, Boolean* constr){
	switch(constr->polarity){
		case P_BOTHTRUEFALSE:
		case P_TRUE:{
			OrderEdge* _1to2 = getOrderEdgeFromOrderGraph(graph, constr, node1, node2);
			addNewOutgoingEdge(node1, _1to2);
			addNewIncomingEdge(node2, _1to2);
			if(constr->polarity == P_TRUE)
				break;
		}
		case P_FALSE:{
			OrderEdge* _2to1 = getOrderEdgeFromOrderGraph(graph, constr, node2, node1);
			addNewOutgoingEdge(node2, _2to1);
			addNewIncomingEdge(node1, _2to1);
			break;
		}
		default:
			ASSERT(0);
				
	}
}

OrderNode* getOrderNodeFromOrderGraph(OrderGraph* graph, uint64_t id, Order* order){
	OrderNode* node = allocOrderNode(id, order);
	OrderNode* tmp = getHashSetOrderNode(graph->nodes, node);
	if( tmp!= NULL){
		deleteOrderNode(node);
		node= tmp;
	} else {
		addHashSetOrderNode(graph->nodes, node);
	}
	return node;
}

OrderEdge* getOrderEdgeFromOrderGraph(OrderGraph* graph, Boolean* order, OrderNode* begin, OrderNode* end){
	OrderEdge* edge = allocOrderEdge(order, begin, end);
	OrderEdge* tmp = getHashSetOrderEdge(graph->edges, edge);
	if(tmp!= NULL){
		deleteOrderEdge(edge);
		edge = tmp;
	} else {
		addHashSetOrderEdge(graph->edges, edge);
	}
	return edge;
}

void addOrderConstraintToOrderGraph(OrderGraph* graph, Boolean* constr){
	BooleanOrder* bOrder = (BooleanOrder*) constr;
	OrderNode* from = getOrderNodeFromOrderGraph(graph, bOrder->first, bOrder->order);
	OrderNode* to = getOrderNodeFromOrderGraph(graph, bOrder->second, bOrder->order);
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