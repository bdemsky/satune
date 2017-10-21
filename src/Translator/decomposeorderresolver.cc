
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

DecomposeOrderResolver::DecomposeOrderResolver(Order * _order) :
	graph(NULL),
	order(_order)
{
}

DecomposeOrderResolver::~DecomposeOrderResolver() {
	if (graph != NULL)
		delete graph;
	uint size=edges.getSize();
	edges.resetAndDelete();
}

void DecomposeOrderResolver::mustOrderEdge(uint64_t first, uint64_t second) {
	DOREdge edge(first, second, 0, first, second);
	if (!edges.contains(&edge)) {
		DOREdge *newedge=new DOREdge(first, second, 0, first, second);
		edges.add(newedge);
	}
}

void DecomposeOrderResolver::remapEdge(uint64_t first, uint64_t second, uint64_t newfirst, uint64_t newsecond) {
	DOREdge edge(first, second, 0, first, second);
	DOREdge *oldedge=edges.get(&edge);
	if (oldedge != NULL) {
		edges.remove(oldedge);
		oldedge->newfirst=newfirst;
		oldedge->newsecond=newsecond;
		edges.add(oldedge);
	} else {
		DOREdge *newedge=new DOREdge(first, second, 0, newfirst, newsecond);
		edges.add(newedge);
	}
}

void DecomposeOrderResolver::setEdgeOrder(uint64_t first, uint64_t second, uint sccNum) {
	DOREdge edge(first, second, 0, first, second);
	DOREdge *oldedge=edges.get(&edge);
	if (oldedge != NULL) {
		oldedge->orderindex=sccNum;
	} else {
		DOREdge *newedge=new DOREdge(first, second, sccNum, first, second);
		edges.add(newedge);
	}
}

void DecomposeOrderResolver::setOrder(uint sccNum, Order *neworder) {
	orders.setExpand(sccNum, neworder);
}

Order * DecomposeOrderResolver::getOrder(uint sccNum) {
	Order *neworder = NULL;
	if (orders.getSize() > sccNum)
		neworder = orders.get(sccNum);
	return neworder;
}

bool DecomposeOrderResolver::resolveOrder(uint64_t first, uint64_t second) {
	OrderNode *from = graph->lookupOrderNodeFromOrderGraph(first);
	ASSERT(from != NULL);
	OrderNode *to = graph->lookupOrderNodeFromOrderGraph(second);
	ASSERT(to != NULL);
	if (from->removed || to->removed) {
		HashsetOrderNode fromset, toset;
		//		processNode(&fromset, from, true);
		//		processNode(&toset, to, false);
		SetIteratorOrderNode *fromit=fromset.iterator();
		while(fromit->hasNext()) {
			OrderNode * nodefrom=fromit->next();
			SetIteratorOrderNode *toit=toset.iterator();
			while(toit->hasNext()) {
				OrderNode * nodeto=toit->next();
				if (resolveOrder(nodefrom->getID(), nodeto->getID())) {
					delete fromit;
					delete toit;
					return true;
				}
			}
			delete toit;
		}
		delete fromit;
		return false;
	} else if (from->sccNum != to->sccNum) {
		OrderEdge *edge = graph->lookupOrderEdgeFromOrderGraph(from, to);
		switch (graph->getOrder()->type) {
		case SATC_TOTAL:
			return from->sccNum < to->sccNum;
		case SATC_PARTIAL:
			return resolvePartialOrder(from, to);
		default:
			ASSERT(0);
		}
	} else {
		Order *suborder = NULL;
		// We should ask this query from the suborder ....
		suborder = orders.get(from->sccNum);
		ASSERT(suborder != NULL);
		return suborder->encoding.resolver->resolveOrder(from->id, to->id);
	}
}

bool DecomposeOrderResolver::resolvePartialOrder(OrderNode *first, OrderNode *second) {
	if (first->sccNum > second->sccNum) {
		return false;
	} else {
		return graph->isTherePath(first, second);
	}

}

