#include "ordergraph.h"
#include "ordernode.h"

OrderGraph* allocOrderGraph(){
	OrderGraph* This = (OrderGraph*) ourmalloc(sizeof(OrderGraph));
	This->nodes = allocHashSetOrderNode(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	return This;
}

void deleteOrderGraph(OrderGraph* graph){
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		deleteOrderNode(node);
	}
	deleteIterOrderNode(iterator);
	ourfree(graph);
}