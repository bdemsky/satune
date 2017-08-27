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
#include "orderencoder.h"
#include "tunable.h"
#include "integerencoding.h"

void orderAnalysis(CSolver *This) {
	Vector<Order *> * orders=This->getOrders();
	uint size = orders->getSize();
	for (uint i = 0; i < size; i++) {
		Order *order = orders->get(i);
		bool doDecompose=GETVARTUNABLE(This->getTuner(), order->type, DECOMPOSEORDER, &onoff);
		if (!doDecompose)
			continue;
		
		OrderGraph *graph = buildOrderGraph(order);
		if (order->type == PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			completePartialOrderGraph(graph);
		}


		bool mustReachGlobal=GETVARTUNABLE(This->getTuner(), order->type, MUSTREACHGLOBAL, &onoff);

		if (mustReachGlobal)
			reachMustAnalysis(This, graph, false);

		bool mustReachLocal=GETVARTUNABLE(This->getTuner(), order->type, MUSTREACHLOCAL, &onoff);
		
		if (mustReachLocal) {
			//This pair of analysis is also optional
			if (order->type == PARTIAL) {
				localMustAnalysisPartial(This, graph);
			} else {
				localMustAnalysisTotal(This, graph);
			}
		}

		bool mustReachPrune=GETVARTUNABLE(This->getTuner(), order->type, MUSTREACHPRUNE, &onoff);
		
		if (mustReachPrune)
			removeMustBeTrueNodes(This, graph);
		
		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);
		decomposeOrder(This, order, graph);
		delete graph;
		
		/*
			OrderIntegerEncodingSATEncoder wants a private field that it really shoukldn't need...

		bool doIntegerEncoding = GETVARTUNABLE(This->getTuner(), order->order.type, ORDERINTEGERENCODING, &offon );
		if(!doIntegerEncoding)
			continue;
		uint size = order->constraints.getSize();
		for(uint i=0; i<size; i++){
			orderIntegerEncodingSATEncoder(This->satEncoder, order->constraints.get(i));
			}*/

	}
}

void decomposeOrder(CSolver *This, Order *order, OrderGraph *graph) {
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
				This->replaceBooleanWithTrue(orderconstraint);
			} else if (edge->polNeg) {
				This->replaceBooleanWithFalse(orderconstraint);
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
				MutableSet *set = This->createMutableSet(order->set->type);
				neworder = This->createOrder(order->type, set);
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
				OrderEdge *edge = graph->getOrderEdgeFromOrderGraph(from, to);
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

