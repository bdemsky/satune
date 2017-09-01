
/* 
 * File:   orderpairresolver.cc
 * Author: hamed
 * 
 * Created on September 1, 2017, 3:36 PM
 */

#include "orderpairresolver.h"
#include "ordergraph.h"
#include "order.h"
#include "orderedge.h"
#include "ordernode.h"
#include "satencoder.h"
#include "csolver.h"

OrderPairResolver::OrderPairResolver(CSolver* _solver, Order* _order) :
	solver(_solver),
	order(_order)
{
}

OrderPairResolver::~OrderPairResolver() {
}

HappenedBefore OrderPairResolver::resolveOrder(uint64_t first, uint64_t second){
	if(order->graph != NULL){
		// For the cases that tuning framework decides no to build a graph for order ...
		OrderGraph* graph = order->graph;
		OrderNode *from = graph->getOrderNodeFromOrderGraph(first, false /*Don't create new node if doesn't exist*/);
		if(from == NULL){
			return SATC_UNORDERED;
		}
		OrderNode *to = graph->getOrderNodeFromOrderGraph(second, false);
		if(from == NULL){
			return SATC_UNORDERED;
		}

		OrderEdge *edge = graph->getOrderEdgeFromOrderGraph(from, to, false /* Don't create a new edge*/);
		if (edge != NULL && edge->mustPos){
			return SATC_FIRST;
		} else if( edge != NULL && edge->mustNeg){
			return SATC_SECOND;
		}
	}

	//Couldn't infer from graph. Should call the SAT Solver ...
	switch( order->type){
		case SATC_TOTAL:
			resolveTotalOrder(first, second);
		case SATC_PARTIAL:
			//TODO: Support for partial order ...
		default:
			ASSERT(0);
	}
	

}


HappenedBefore OrderPairResolver::resolveTotalOrder(uint64_t first, uint64_t second) {
	ASSERT(order->orderPairTable != NULL);
	OrderPair pair(first, second, E_NULL);
	Edge var = getOrderConstraint(order->orderPairTable, &pair);
	if (edgeIsNull(var))
		return SATC_UNORDERED;
	return getValueCNF(solver->getSATEncoder()->getCNF(), var) ? SATC_FIRST : SATC_SECOND;
}
