#include "orderresolver.h"
#include "ordergraph.h"
#include "ordernode.h"
#include "orderedge.h"

OrderResolver::OrderResolver(OrderGraph* _graph)
	:graph(_graph)
{
}

OrderResolver::~OrderResolver(){
	delete graph;
}

HappenedBefore OrderResolver::resolveOrder(uint64_t first, uint64_t second){
	OrderNode *from = graph->getOrderNodeFromOrderGraph(first, false /*Don't create new node if doesn't exist*/);
	if(from == NULL){
		return SATC_UNORDERED;
	}
	OrderNode *to = graph->getOrderNodeFromOrderGraph(second, false);
	if(from == NULL){
		return SATC_UNORDERED;
	}
	if (from->sccNum != to->sccNum) {
		OrderEdge *edge = graph->getOrderEdgeFromOrderGraph(from, to, false /* Don't create a new edge*/);
		if (edge != NULL && edge->mustPos){
			return SATC_FIRST;
		} else if( edge != NULL && edge->mustNeg){
			return SATC_SECOND;
		}else {
			ASSERT(0);
			//It's a case that either there's no edge, or there is an edge
			// but we don't know the value! (This case shouldn't happen)
			//return from->sccNum < to->sccNum? SATC_FIRST: SATC_SECOND;
		}
	} else {
		return getOrder(from, to);
	}
}