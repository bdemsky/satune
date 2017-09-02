
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

HappenedBefore DecomposeOrderResolver::resolveOrder(uint64_t first, uint64_t second) {
	OrderNode *from = graph->getOrderNodeFromOrderGraph(first, false /*Don't create new node if doesn't exist*/);
	if (from == NULL) {
		return SATC_UNORDERED;
	}
	OrderNode *to = graph->getOrderNodeFromOrderGraph(second, false);
	if (from == NULL) {
		return SATC_UNORDERED;
	}
	if (from->sccNum != to->sccNum) {
		OrderEdge *edge = graph->getOrderEdgeFromOrderGraph(from, to, false	/* Don't create a new edge*/);
		if (edge != NULL && edge->mustPos) {
			return SATC_FIRST;
		} else if ( edge != NULL && edge->mustNeg) {
			return SATC_SECOND;
		} else {
			switch (graph->getOrder()->type) {
			case SATC_TOTAL:
				return from->sccNum < to->sccNum ? SATC_FIRST : SATC_SECOND;
			case SATC_PARTIAL:
			//Adding support for partial order ...
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

