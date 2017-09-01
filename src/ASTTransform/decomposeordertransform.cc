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
#include "decomposeorderresolver.h"


DecomposeOrderTransform::DecomposeOrderTransform(CSolver* _solver)
	:Transform(_solver)
{
}

DecomposeOrderTransform::~DecomposeOrderTransform() {
}

bool DecomposeOrderTransform::canExecuteTransform(){
	return canExecutePass(solver, currOrder->type, DECOMPOSEORDER, &onoff);
}

void DecomposeOrderTransform::doTransform(){
	Vector<Order *> ordervec;
	Vector<Order *> partialcandidatevec;
	uint size = currOrder->constraints.getSize();
	for (uint i = 0; i < size; i++) {
		BooleanOrder *orderconstraint = currOrder->constraints.get(i);
		OrderNode *from = currGraph->getOrderNodeFromOrderGraph(orderconstraint->first);
		OrderNode *to = currGraph->getOrderNodeFromOrderGraph(orderconstraint->second);
		model_print("from->sccNum:%u\tto->sccNum:%u\n", from->sccNum, to->sccNum);
		if (from->sccNum != to->sccNum) {
			OrderEdge *edge = currGraph->getOrderEdgeFromOrderGraph(from, to);
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
				MutableSet *set = solver->createMutableSet(currOrder->set->getType());
				neworder = solver->createOrder(currOrder->type, set);
				ordervec.setExpand(from->sccNum, neworder);
				if (currOrder->type == SATC_PARTIAL)
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
			if (currOrder->type == SATC_PARTIAL) {
				OrderEdge *edge = currGraph->getOrderEdgeFromOrderGraph(from, to);
				if (edge->polNeg)
					partialcandidatevec.setExpand(from->sccNum, NULL);
			}
			orderconstraint->order = neworder;
			neworder->addOrderConstraint(orderconstraint);
		}
	}
	currOrder->setOrderResolver( new DecomposeOrderResolver(currGraph, ordervec) );
	uint pcvsize = partialcandidatevec.getSize();
	for (uint i = 0; i < pcvsize; i++) {
		Order *neworder = partialcandidatevec.get(i);
		if (neworder != NULL) {
			neworder->type = SATC_TOTAL;
			model_print("i=%u\t", i);
		}
	}
}
