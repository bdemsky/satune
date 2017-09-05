
/*
 * File:   DecomposeOrderResolver.cc
 * Author: hamed
 *
 * Created on September 1, 2017, 10:36 AM
 */

#include "decomposeorderresolver.h"
#include "order.h"
#include "ordernode.h"
#include "ordergraph.h"

DecomposeOrderResolver::DecomposeOrderResolver(OrderGraph *_graph, Vector<Order *> &_orders) :
	graph(_graph),
	orders(_orders.getSize(), _orders.expose())
{
}

DecomposeOrderResolver::~DecomposeOrderResolver() {
}

bool DecomposeOrderResolver::resolveOrder(uint64_t first, uint64_t second) {
	OrderNode *from = graph->lookupOrderNodeFromOrderGraph(first);
	ASSERT(from != NULL);
	OrderNode *to = graph->lookupOrderNodeFromOrderGraph(second);
	ASSERT(to != NULL);

	if (from->sccNum != to->sccNum) {
		OrderEdge *edge = graph->lookupOrderEdgeFromOrderGraph(from, to);
		if (edge != NULL && edge->mustPos) {
			return true;
		} else if ( edge != NULL && edge->mustNeg) {
			return false;
		} else {
			switch (graph->getOrder()->type) {
			case SATC_TOTAL:
				return from->sccNum < to->sccNum;
			case SATC_PARTIAL:
				return resolvePartialOrder(from, to);
			default:
				ASSERT(0);
			}
		}
	} else {
		Order *suborder = NULL;
		// We should ask this query from the suborder ....
		suborder = orders.get(from->sccNum);
		ASSERT(suborder != NULL);
		return suborder->encoding.resolver->resolveOrder(from->id, to->id);
	}
}

bool DecomposeOrderResolver::resolvePartialOrder(OrderNode* first, OrderNode* second){
	if(first->sccNum > second->sccNum){
		return false;
	} else {
		return graph->isTherePath(first, second);
	}
		
}

