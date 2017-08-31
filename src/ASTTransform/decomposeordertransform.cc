/* 
 * File:   ordertransform.cc
 * Author: hamed
 * 
 * Created on August 28, 2017, 10:35 AM
 */

#include "decomposeordertransform.h"
#include "order.h"
#include "orderedge.h"
#include "ordernode.h"
#include "boolean.h"
#include "mutableset.h"
#include "ordergraph.h"
#include "csolver.h"


DecomposeOrderTransform::DecomposeOrderTransform(CSolver* _solver, Order* _order)
	:Transform(_solver),
	order(_order)
{
}

DecomposeOrderTransform::~DecomposeOrderTransform() {
}

bool DecomposeOrderTransform::canExecuteTransform(){
	return canExecutePass(solver, order->type, DECOMPOSEORDER, &onoff);
}

void DecomposeOrderTransform::doTransform(){
	Vector<Order *> ordervec;
	Vector<Order *> partialcandidatevec;
	uint size = order->constraints.getSize();
	for (uint i = 0; i < size; i++) {
		BooleanOrder *orderconstraint = order->constraints.get(i);
		OrderNode *from = graph->getOrderNodeFromOrderGraph(orderconstraint->first);
		OrderNode *to = graph->getOrderNodeFromOrderGraph(orderconstraint->second);
		model_print("from->sccNum:%u\tto->sccNum:%u\n", from->sccNum, to->sccNum);
		if (from->sccNum != to->sccNum) {
			OrderEdge *edge = graph->getOrderEdgeFromOrderGraph(from, to);
			if (edge->polPos) {
				solver->replaceBooleanWithTrue(orderconstraint);
			} else if (edge->polNeg) {
				solver->replaceBooleanWithFalse(orderconstraint);
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
				MutableSet *set = solver->createMutableSet(order->set->getType());
				neworder = solver->createOrder(order->type, set);
				ordervec.setExpand(from->sccNum, neworder);
				if (order->type == SATC_PARTIAL)
					partialcandidatevec.setExpand(from->sccNum, neworder);
				else
					partialcandidatevec.setExpand(from->sccNum, NULL);
			}
			if (from->status != SATC_ADDEDTOSET) {
				from->status = SATC_ADDEDTOSET;
				((MutableSet *)neworder->set)->addElementMSet(from->id);
			}
			if (to->status != SATC_ADDEDTOSET) {
				to->status = SATC_ADDEDTOSET;
				((MutableSet *)neworder->set)->addElementMSet(to->id);
			}
			if (order->type == SATC_PARTIAL) {
				OrderEdge *edge = graph->getOrderEdgeFromOrderGraph(from, to);
				if (edge->polNeg)
					partialcandidatevec.setExpand(from->sccNum, NULL);
			}
			orderconstraint->order = neworder;
			neworder->addOrderConstraint(orderconstraint);
		}
	}

	uint pcvsize = partialcandidatevec.getSize();
	for (uint i = 0; i < pcvsize; i++) {
		Order *neworder = partialcandidatevec.get(i);
		if (neworder != NULL) {
			neworder->type = SATC_TOTAL;
			model_print("i=%u\t", i);
		}
	}
}
