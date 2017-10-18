
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

OrderPairResolver::OrderPairResolver(CSolver *_solver, Order *_order) :
	solver(_solver),
	order(_order),
	orderPairTable(new HashtableOrderPair())
{
}

OrderPairResolver::~OrderPairResolver() {
	if (orderPairTable != NULL) {
		orderPairTable->resetanddelete();
		delete orderPairTable;
	}
}

bool OrderPairResolver::resolveOrder(uint64_t first, uint64_t second) {
	if (order->graph != NULL) {
		// For the cases that tuning framework decides no to build a graph for order ...
		OrderGraph *graph = order->graph;
		OrderNode *from = graph->lookupOrderNodeFromOrderGraph(first);
		ASSERT(from != NULL);
		OrderNode *to = graph->lookupOrderNodeFromOrderGraph(second);
		ASSERT(to != NULL);

		OrderEdge *edge = graph->lookupOrderEdgeFromOrderGraph(from, to);

		if (edge != NULL && edge->mustPos) {
			return true;
		} else if ( edge != NULL && edge->mustNeg) {
			return false;
		}
	}

	//Couldn't infer from graph. Should call the SAT Solver ...
	return getOrderConstraintValue(first, second);
}

bool OrderPairResolver::getOrderConstraintValue(uint64_t first, uint64_t second) {
	ASSERT(first != second);
	bool negate = false;
	OrderPair tmp(first, second);
	if (first < second) {
		negate = true;
		tmp.first = second;
		tmp.second = first;
	}
	if (!orderPairTable->contains(&tmp)) {
		return false;
	}
	OrderPair *pair = orderPairTable->get(&tmp);
	return negate ? pair->getNegatedConstraintValue(solver) : pair->getConstraintValue(solver);
}
