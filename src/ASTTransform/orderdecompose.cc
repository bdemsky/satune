#include "orderdecompose.h"
#include "common.h"
#include "order.h"
#include "boolean.h"
#include "ordergraph.h"
#include "ordernode.h"
#include "rewriter.h"
#include "orderedge.h"
#include "mutableset.h"
#include "ops.h"
#include "csolver.h"

void decomposeOrder(CSolver *This, Order *order, OrderGraph *graph) {
	Vector<Order *> ordervec;
	Vector<Order *> partialcandidatevec;
	uint size = order->constraints.getSize();
	for (uint i = 0; i < size; i++) {
		BooleanOrder *orderconstraint = order->constraints.get(i);
		OrderNode *from = getOrderNodeFromOrderGraph(graph, orderconstraint->first);
		OrderNode *to = getOrderNodeFromOrderGraph(graph, orderconstraint->second);
		model_print("from->sccNum:%u\tto->sccNum:%u\n", from->sccNum, to->sccNum);
		if (from->sccNum != to->sccNum) {
			OrderEdge *edge = getOrderEdgeFromOrderGraph(graph, from, to);			
			if (edge->polPos) {
				replaceBooleanWithTrue(This, (Boolean *)orderconstraint);
			} else if (edge->polNeg) {
				replaceBooleanWithFalse(This, (Boolean *)orderconstraint);
			} else {
				//This case should only be possible if constraint isn't in AST
				ASSERT(0);
			}
		} else {
			//Build new order and change constraint's order
			Order *neworder = NULL;
			if (ordervec.getSize() > from->sccNum)
				neworder = ordervec.get(from->sccNum);
			if (neworder == NULL) {
				MutableSet *set = new MutableSet(order->set->type);
				neworder = new Order(order->type, set);
				This->allOrders.push(neworder);
				ordervec.setExpand(from->sccNum, neworder);
				if (order->type == PARTIAL)
					partialcandidatevec.setExpand(from->sccNum, neworder);
				else
					partialcandidatevec.setExpand(from->sccNum, NULL);
			}
			if (from->status != ADDEDTOSET) {
				from->status = ADDEDTOSET;
				((MutableSet *)neworder->set)->addElementMSet(from->id);
			}
			if (to->status != ADDEDTOSET) {
				to->status = ADDEDTOSET;
				((MutableSet *)neworder->set)->addElementMSet(to->id);
			}
			if (order->type == PARTIAL) {
				OrderEdge *edge = getOrderEdgeFromOrderGraph(graph, from, to);
				if (edge->polNeg)
					partialcandidatevec.setExpand(from->sccNum, NULL);
			}
			orderconstraint->order = neworder;
			neworder->addOrderConstraint(orderconstraint);
		}
	}

	uint pcvsize=partialcandidatevec.getSize();
	for(uint i=0;i<pcvsize;i++) {
		Order * neworder=partialcandidatevec.get(i);
		if (neworder != NULL){
			neworder->type = TOTAL;
			model_print("i=%u\t", i);
		}
	}
}

