
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

DecomposeOrderResolver::DecomposeOrderResolver(Order *_order) :
	graph(NULL),
	order(_order)
{
}

DecomposeOrderResolver::~DecomposeOrderResolver() {
	if (graph != NULL)
		delete graph;
	edges.resetAndDelete();
}

void DecomposeOrderResolver::mustOrderEdge(uint64_t first, uint64_t second) {
	DOREdge edge(first, second, 0, first, second);
	DOREdge *oldedge = edges.get(&edge);
	if (oldedge != NULL) {
		oldedge->mustbetrue = true;
	} else {
		DOREdge *newedge = new DOREdge(first, second, 0, first, second);
		newedge->mustbetrue = true;
		edges.add(newedge);
	}
}

void DecomposeOrderResolver::remapEdge(uint64_t first, uint64_t second, uint64_t newfirst, uint64_t newsecond) {
	DOREdge edge(first, second, 0, first, second);
	DOREdge *oldedge = edges.get(&edge);
	if (oldedge != NULL) {
		edges.remove(oldedge);
		oldedge->newfirst = newfirst;
		oldedge->newsecond = newsecond;
		edges.add(oldedge);
	} else {
		DOREdge *newedge = new DOREdge(first, second, 0, newfirst, newsecond);
		edges.add(newedge);
	}
}

void DecomposeOrderResolver::setEdgeOrder(uint64_t first, uint64_t second, uint sccNum) {
	DOREdge edge(first, second, 0, first, second);
	DOREdge *oldedge = edges.get(&edge);
	if (oldedge != NULL) {
		oldedge->orderindex = sccNum;
	} else {
		DOREdge *newedge = new DOREdge(first, second, sccNum, first, second);
		edges.add(newedge);
	}
	/* Also fix up reverse edge if it exists */
	DOREdge revedge(second, first, 0, second, first);
	oldedge = edges.get(&revedge);
	if (oldedge != NULL) {
		oldedge->orderindex = sccNum;
	}
}

void DecomposeOrderResolver::setOrder(uint sccNum, Order *neworder) {
	orders.setExpand(sccNum, neworder);
}

Order *DecomposeOrderResolver::getOrder(uint sccNum) {
	Order *neworder = NULL;
	if (orders.getSize() > sccNum)
		neworder = orders.get(sccNum);
	return neworder;
}

void DecomposeOrderResolver::buildGraph() {
	graph = new OrderGraph(order);
	SetIteratorDOREdge *iterator = edges.iterator();
	while (iterator->hasNext()) {
		DOREdge *doredge = iterator->next();
		if (doredge->mustbetrue) {
			graph->addEdge(doredge->origfirst, doredge->origsecond);
			if (doredge->newfirst != doredge->origfirst || doredge->newsecond != doredge->origsecond) {
				graph->addEdge(doredge->newfirst, doredge->newsecond);
			}
		} else if (doredge->orderindex != 0) {
			Order *suborder = orders.get(doredge->orderindex);
			bool isEdge = suborder->encoding.resolver->resolveOrder(doredge->newfirst, doredge->newsecond);
			if (isEdge)
				graph->addEdge(doredge->origfirst, doredge->origsecond);
			else if (order->type == SATC_TOTAL)
				graph->addEdge(doredge->origsecond, doredge->origfirst);
		}
	}
	delete iterator;
	if (order->type == SATC_TOTAL) {
		graph->computeStronglyConnectedComponentGraph();
	}
}

bool DecomposeOrderResolver::resolveOrder(uint64_t first, uint64_t second) {
	if (graph == NULL)
		buildGraph();

	OrderNode *from = graph->lookupOrderNodeFromOrderGraph(first);
	if (from == NULL) {
		ASSERT(order->type != SATC_TOTAL);
		return false;
	}
	OrderNode *to = graph->lookupOrderNodeFromOrderGraph(second);
	if (to == NULL) {
		ASSERT(order->type != SATC_TOTAL);
		return false;
	}
	switch (order->type) {
	case SATC_TOTAL:
		return from->sccNum < to->sccNum;
	case SATC_PARTIAL:
		return resolvePartialOrder(from, to);
	default:
		ASSERT(0);
	}
}

bool DecomposeOrderResolver::resolvePartialOrder(OrderNode *first, OrderNode *second) {
	if (first->sccNum > second->sccNum) {
		return false;
	} else {
		return graph->isTherePath(first, second);
	}
}

