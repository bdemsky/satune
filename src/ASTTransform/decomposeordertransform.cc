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
#include "tunable.h"
#include "orderanalysis.h"


DecomposeOrderTransform::DecomposeOrderTransform(CSolver *_solver)
	: Transform(_solver)
{
}

DecomposeOrderTransform::~DecomposeOrderTransform() {
}

void DecomposeOrderTransform::doTransform() {
	HashsetOrder *orders = solver->getActiveOrders()->copy();
	SetIteratorOrder * orderit=orders->iterator();
	while(orderit->hasNext()) {
		Order *order = orderit->next();

		if (GETVARTUNABLE(solver->getTuner(), order->type, DECOMPOSEORDER, &onoff) == 0) {
			continue;
		}

		OrderGraph *graph = buildOrderGraph(order);
		if (order->type == SATC_PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			completePartialOrderGraph(graph);
		}

		bool mustReachGlobal = GETVARTUNABLE(solver->getTuner(), order->type, MUSTREACHGLOBAL, &onoff);

		if (mustReachGlobal)
			reachMustAnalysis(solver, graph, false);

		bool mustReachLocal = GETVARTUNABLE(solver->getTuner(), order->type, MUSTREACHLOCAL, &onoff);

		if (mustReachLocal) {
			//This pair of analysis is also optional
			if (order->type == SATC_PARTIAL) {
				localMustAnalysisPartial(solver, graph);
			} else {
				localMustAnalysisTotal(solver, graph);
			}
		}

		
		bool mustReachPrune = GETVARTUNABLE(solver->getTuner(), order->type, MUSTREACHPRUNE, &onoff);

		if (mustReachPrune)
			removeMustBeTrueNodes(solver, graph);
		
		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);
		decomposeOrder(order, graph);
	}
	delete orderit;
	delete orders;
}


void DecomposeOrderTransform::decomposeOrder (Order *currOrder, OrderGraph *currGraph) {
	Vector<Order *> ordervec;
	Vector<Order *> partialcandidatevec;
	uint size = currOrder->constraints.getSize();
	for (uint i = 0; i < size; i++) {
		BooleanOrder *orderconstraint = currOrder->constraints.get(i);
		OrderNode *from = currGraph->getOrderNodeFromOrderGraph(orderconstraint->first);
		OrderNode *to = currGraph->getOrderNodeFromOrderGraph(orderconstraint->second);
		if (from->sccNum != to->sccNum) {
			OrderEdge *edge = currGraph->lookupOrderEdgeFromOrderGraph(from, to);
			if (edge != NULL) {
				if (edge->polPos) {
					solver->replaceBooleanWithTrue(orderconstraint);
				} else if (edge->polNeg) {
					solver->replaceBooleanWithFalse(orderconstraint);
				} else {
					//This case should only be possible if constraint isn't in AST
					//This can happen, so don't do anything
					;
				}
			} else {
				OrderEdge *invedge = currGraph->lookupOrderEdgeFromOrderGraph(to, from);
				if (invedge != NULL) {
					if (invedge->polPos) {
						solver->replaceBooleanWithFalse(orderconstraint);
					} else if (edge->polNeg) {
						//This case shouldn't happen...  If we have a partial order,
						//then we should have our own edge...If we have a total
						//order, then this edge should be positive...
						ASSERT(0);
					} else {
						//This case should only be possible if constraint isn't in AST
						//This can happen, so don't do anything
						;
					}
				}
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
	solver->getActiveOrders()->remove(currOrder);
	uint pcvsize = partialcandidatevec.getSize();
	for (uint i = 0; i < pcvsize; i++) {
		Order *neworder = partialcandidatevec.get(i);
		if (neworder != NULL) {
			neworder->type = SATC_TOTAL;
		}
	}
}
