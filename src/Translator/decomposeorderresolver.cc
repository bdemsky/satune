
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

void processNode(HashsetOrderNode * set, OrderNode *node, bool outedges) {
	if (node->removed) {
		Vector<OrderNode *> toprocess;
		HashsetOrderNode visited;
		toprocess.push(node);
		while(toprocess.getSize()!=0) {
			OrderNode *newnode=toprocess.last();toprocess.pop();
			SetIteratorOrderEdge *iterator=outedges ? newnode->outEdges.iterator() : newnode->inEdges.iterator();
			while(iterator->hasNext()) {
				OrderEdge *edge=iterator->next();
				OrderNode *tmpnode=outedges ? edge->sink : edge->source;
				if (tmpnode->removed) {
					if (visited.add(tmpnode)) {
						toprocess.push(tmpnode);
					}
				} else {
					set->add(tmpnode);
				}
			}
			delete iterator;
		}
	} else
		set->add(node);
}

bool DecomposeOrderResolver::resolveOrder(uint64_t first, uint64_t second) {
	OrderNode *from = graph->lookupOrderNodeFromOrderGraph(first);
	ASSERT(from != NULL);
	OrderNode *to = graph->lookupOrderNodeFromOrderGraph(second);
	ASSERT(to != NULL);
	if (from->removed || to->removed) {
		HashsetOrderNode fromset, toset;
		processNode(&fromset, from, true);
		processNode(&toset, to, false);
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

