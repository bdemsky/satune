
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

DecomposeOrderResolver::DecomposeOrderResolver(OrderGraph* _graph, Vector<Order*>& _orders):
	OrderResolver(_graph),
	orders(_orders.getSize(), _orders.expose())
{
}

DecomposeOrderResolver::~DecomposeOrderResolver() {
	delete graph;
}

HappenedBefore DecomposeOrderResolver::getOrder(OrderNode* from, OrderNode* to){
	ASSERT(from->id == to->id);
	// We should ask this query from the suborder ....
	Order *suborder = NULL;
	suborder = orders.get(from->sccNum);
	ASSERT(suborder != NULL);
	return suborder->encoding.resolver->resolveOrder(from->id, to->id);
}

