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
	SetIteratorOrder *orderit = orders->iterator();
	while (orderit->hasNext()) {
		Order *order = orderit->next();

		if (GETVARTUNABLE(solver->getTuner(), order->type, DECOMPOSEORDER, &onoff) == 0) {
			continue;
		}

		DecomposeOrderResolver *dor=new DecomposeOrderResolver(order);
		order->setOrderResolver(dor);
		
		OrderGraph *graph = buildOrderGraph(order);
		if (order->type == SATC_PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			graph->completePartialOrderGraph();
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
		HashsetOrderEdge *edgesRemoved = NULL;

		if (mustReachPrune) {
			edgesRemoved = new HashsetOrderEdge();
			removeMustBeTrueNodes(graph, edgesRemoved);
		}

		//This is needed for splitorder
		graph->computeStronglyConnectedComponentGraph();
		decomposeOrder(order, graph, edgesRemoved, dor);
		if (edgesRemoved != NULL)
			delete edgesRemoved;
	}
	delete orderit;
	delete orders;
}


void DecomposeOrderTransform::decomposeOrder (Order *currOrder, OrderGraph *currGraph, HashsetOrderEdge *edgesRemoved, DecomposeOrderResolver *dor) {
	Vector<Order *> partialcandidatevec;
	uint size = currOrder->constraints.getSize();
	for (uint i = 0; i < size; i++) {
		BooleanOrder *orderconstraint = currOrder->constraints.get(i);
		OrderNode *from = currGraph->getOrderNodeFromOrderGraph(orderconstraint->first);
		OrderNode *to = currGraph->getOrderNodeFromOrderGraph(orderconstraint->second);
		OrderEdge *edge = currGraph->lookupOrderEdgeFromOrderGraph(from, to);
		OrderEdge *invedge = currGraph->lookupOrderEdgeFromOrderGraph(to, from);
		if (edgesRemoved != NULL) {
			if (edgesRemoved->contains(edge)) {
				dor->mustOrderEdge(from->getID(), to->getID());
				solver->replaceBooleanWithTrue(orderconstraint);
				continue;
			} else if (edgesRemoved->contains(invedge)) {
				dor->mustOrderEdge(to->getID(), from->getID());
				solver->replaceBooleanWithFalse(orderconstraint);
				continue;
			}
		}

		if (from->sccNum != to->sccNum) {
			if (edge != NULL) {
				if (edge->polPos) {
					dor->mustOrderEdge(from->getID(), to->getID());
					solver->replaceBooleanWithTrue(orderconstraint);
				} else if (edge->polNeg) {
					if (currOrder->type == SATC_TOTAL)					
						dor->mustOrderEdge(to->getID(), from->getID());
					solver->replaceBooleanWithFalse(orderconstraint);
				} else {
					//This case should only be possible if constraint isn't in AST
					//This can happen, so don't do anything
					;
				}
			} else {
				if (invedge != NULL) {
					if (invedge->polPos) {
						dor->mustOrderEdge(to->getID(), from->getID());
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
			neworder = dor->getOrder(from->sccNum);
			if (neworder == NULL) {
				MutableSet *set = solver->createMutableSet(currOrder->set->getType());
				neworder = solver->createOrder(currOrder->type, set);
				dor->setOrder(from->sccNum, neworder);
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
				if (edge->polNeg)
					partialcandidatevec.setExpand(from->sccNum, NULL);
			}
			orderconstraint->order = neworder;
			dor->setEdgeOrder(from->getID(), to->getID(), from->sccNum);
			neworder->addOrderConstraint(orderconstraint);
		}
	}
	solver->getActiveOrders()->remove(currOrder);
	uint pcvsize = partialcandidatevec.getSize();
	for (uint i = 0; i < pcvsize; i++) {
		Order *neworder = partialcandidatevec.get(i);
		if (neworder != NULL) {
			neworder->type = SATC_TOTAL;
		}
	}
}

bool DecomposeOrderTransform::isMustBeTrueNode(OrderNode *node) {
	SetIteratorOrderEdge *iterator = node->inEdges.iterator();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (!edge->mustPos) {
			delete iterator;
			return false;
		}
	}
	delete iterator;
	iterator = node->outEdges.iterator();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (!edge->mustPos) {
			delete iterator;
			return false;
		}
	}
	delete iterator;
	return true;
}

void DecomposeOrderTransform::bypassMustBeTrueNode(OrderGraph *graph, OrderNode *node, HashsetOrderEdge *edgesRemoved) {
	node->removed = true;
	SetIteratorOrderEdge *iterin = node->inEdges.iterator();
	while (iterin->hasNext()) {
		OrderEdge *inEdge = iterin->next();
		OrderNode *srcNode = inEdge->source;
		srcNode->outEdges.remove(inEdge);
		edgesRemoved->add(inEdge);
		SetIteratorOrderEdge *iterout = node->outEdges.iterator();
		while (iterout->hasNext()) {
			OrderEdge *outEdge = iterout->next();
			OrderNode *sinkNode = outEdge->sink;
			sinkNode->inEdges.remove(outEdge);
			edgesRemoved->add(outEdge);
			//Adding new edge to new sink and src nodes ...
			if (srcNode == sinkNode) {
				solver->setUnSAT();
				delete iterout;
				delete iterin;
				graph->removeNode(node);
				return;
			}
			//Add new order constraint
			BooleanEdge orderconstraint = solver->orderConstraint(graph->getOrder(), srcNode->getID(), sinkNode->getID());
			solver->addConstraint(orderconstraint);

			//Add new edge
			OrderEdge *newEdge = graph->getOrderEdgeFromOrderGraph(srcNode, sinkNode);
			newEdge->mustPos = true;
			newEdge->polPos = true;
			if (newEdge->mustNeg)
				solver->setUnSAT();
			srcNode->outEdges.add(newEdge);
			sinkNode->inEdges.add(newEdge);
		}
		delete iterout;
	}
	delete iterin;
	graph->removeNode(node);
}

void DecomposeOrderTransform::removeMustBeTrueNodes(OrderGraph *graph, HashsetOrderEdge *edgesRemoved) {
	SetIteratorOrderNode *iterator = graph->getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = iterator->next();
		if (isMustBeTrueNode(node)) {
			bypassMustBeTrueNode(graph, node, edgesRemoved);
		}
	}
	delete iterator;
}
