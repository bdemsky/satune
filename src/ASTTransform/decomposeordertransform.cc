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
#include "polarityassignment.h"


DecomposeOrderTransform::DecomposeOrderTransform(CSolver *_solver)
	: Transform(_solver)
{
}

DecomposeOrderTransform::~DecomposeOrderTransform() {
}

void DecomposeOrderTransform::doTransform() {
	if (solver->isUnSAT())
		return;
	HashsetOrder *orders = solver->getActiveOrders()->copy();
	SetIteratorOrder *orderit = orders->iterator();
	while (orderit->hasNext()) {
		Order *order = orderit->next();

		if (GETVARTUNABLE(solver->getTuner(), order->type, DECOMPOSEORDER, &onoff) == 0) {
			continue;
		}

		DecomposeOrderResolver *dor = new DecomposeOrderResolver(order);
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

		if (mustReachPrune) {
			removeMustBeTrueNodes(graph, dor);
		}

		bool pruneEdges = GETVARTUNABLE(solver->getTuner(), order->type, MUSTEDGEPRUNE, &onoff);

		if (pruneEdges) {
			mustEdgePrune(graph, dor);
		}

		//This is needed for splitorder
		graph->computeStronglyConnectedComponentGraph();
		decomposeOrder(order, graph, dor);
		delete graph;
	}
	delete orderit;
	delete orders;
}


void DecomposeOrderTransform::decomposeOrder(Order *currOrder, OrderGraph *currGraph, DecomposeOrderResolver *dor) {
	Vector<Order *> partialcandidatevec;
	Vector<BooleanOrder *> *constraints = currOrder->getConstraints();
	uint size = constraints->getSize();
	for (uint i = 0; i < size; i++) {
		BooleanOrder *orderconstraint = constraints->get(i);
		OrderNode *from = currGraph->getOrderNodeFromOrderGraph(orderconstraint->first);
		OrderNode *to = currGraph->getOrderNodeFromOrderGraph(orderconstraint->second);
		OrderEdge *edge = currGraph->lookupOrderEdgeFromOrderGraph(from, to);
		OrderEdge *invedge = currGraph->lookupOrderEdgeFromOrderGraph(to, from);
		if (from->removed || to->removed)
			continue;

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
			BooleanEdge neworderconstraint = solver->orderConstraint(neworder, orderconstraint->first, orderconstraint->second);
			solver->replaceBooleanWithBoolean(orderconstraint, neworderconstraint);
			updateEdgePolarity(neworderconstraint, orderconstraint);
			dor->setEdgeOrder(from->getID(), to->getID(), from->sccNum);
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

void DecomposeOrderTransform::bypassMustBeTrueNode(OrderGraph *graph, OrderNode *node, DecomposeOrderResolver *dor) {
	node->removed = true;
	SetIteratorOrderEdge *iterin = node->inEdges.iterator();
	while (iterin->hasNext()) {
		OrderEdge *inEdge = iterin->next();
		OrderNode *srcNode = inEdge->source;
		srcNode->outEdges.remove(inEdge);
		dor->mustOrderEdge(srcNode->getID(), node->getID());
		BooleanEdge be = solver->orderConstraint(graph->getOrder(), srcNode->getID(), node->getID());
		solver->replaceBooleanWithTrue(be);

		SetIteratorOrderEdge *iterout = node->outEdges.iterator();
		while (iterout->hasNext()) {
			OrderEdge *outEdge = iterout->next();
			OrderNode *sinkNode = outEdge->sink;
			//Adding new edge to new sink and src nodes ...
			if (srcNode == sinkNode) {
				solver->setUnSAT();
				delete iterout;
				delete iterin;
				return;
			}
			//Add new order constraint
			BooleanEdge orderconstraint = solver->orderConstraint(graph->getOrder(), srcNode->getID(), sinkNode->getID());
			updateEdgePolarity(orderconstraint, P_TRUE);
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

	//Clean up old edges...  Keep this later in case we don't have any in edges
	SetIteratorOrderEdge *iterout = node->outEdges.iterator();
	while (iterout->hasNext()) {
		OrderEdge *outEdge = iterout->next();
		OrderNode *sinkNode = outEdge->sink;
		dor->mustOrderEdge(node->getID(), sinkNode->getID());
		sinkNode->inEdges.remove(outEdge);
		BooleanEdge be2 = solver->orderConstraint(graph->getOrder(), node->getID(), sinkNode->getID());
		solver->replaceBooleanWithTrue(be2);
	}
	delete iterout;
}

void DecomposeOrderTransform::removeMustBeTrueNodes(OrderGraph *graph, DecomposeOrderResolver *dor) {
	SetIteratorOrderNode *iterator = graph->getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = (OrderNode *)iterator->next();
		if (node->removed)
			continue;
		if (isMustBeTrueNode(node)) {
			bypassMustBeTrueNode(graph, node, dor);
		}
	}
	delete iterator;
}

void DecomposeOrderTransform::mustEdgePrune(OrderGraph *graph, DecomposeOrderResolver *dor) {
	SetIteratorOrderNode *iterator = graph->getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = (OrderNode *)iterator->next();
		if (node->removed)
			continue;
		attemptNodeMerge(graph, node, dor);
	}
	delete iterator;
}

void DecomposeOrderTransform::attemptNodeMerge(OrderGraph *graph, OrderNode *node, DecomposeOrderResolver *dor) {
	SetIteratorOrderEdge *edgeit = node->outEdges.iterator();
	while (edgeit->hasNext()) {
		OrderEdge *outedge = edgeit->next();
		//Only eliminate must edges
		if (!outedge->mustPos)
			continue;
		OrderNode *dstnode = outedge->sink;
		uint numOutEdges = node->outEdges.getSize();
		uint numInEdges = dstnode->inEdges.getSize();
		/*
		   Need to avoid a situation where we create new reachability by
		   the merge.  This can only happen if there is an extra in edge to
		   the dstnode and an extra out edge to our node.
		 */

		if (numOutEdges == 1 || numInEdges == 1) {
			/* Safe to do the Merge */
			mergeNodes(graph, node, outedge, dstnode, dor);

			//Throw away the iterator and start over
			delete edgeit;
			edgeit = node->outEdges.iterator();
		}
	}
	delete edgeit;
}

void DecomposeOrderTransform::mergeNodes(OrderGraph *graph, OrderNode *node, OrderEdge *edge, OrderNode *dstnode, DecomposeOrderResolver *dor) {
	/* Fix up must edge between the two nodes */
	node->outEdges.remove(edge);
	dstnode->inEdges.remove(edge);
	dor->mustOrderEdge(node->getID(), dstnode->getID());

	BooleanEdge be = solver->orderConstraint(graph->getOrder(), node->getID(), dstnode->getID());
	solver->replaceBooleanWithTrue(be);

	/* Go through the incoming edges to the new node */
	SetIteratorOrderEdge *inedgeit = dstnode->inEdges.iterator();
	while (inedgeit->hasNext()) {
		OrderEdge *inedge = inedgeit->next();
		OrderNode *source = inedge->source;
		//remove it from the source node
		source->outEdges.remove(inedge);
		//save the remapping that we did
		dor->remapEdge(source->getID(), dstnode->getID(), source->getID(), node->getID());
		//create the new edge
		OrderEdge *newedge = graph->getOrderEdgeFromOrderGraph(source, node);
		//update the flags
		newedge->polPos |= inedge->polPos;
		newedge->polNeg |= inedge->polNeg;
		newedge->mustPos |= inedge->mustPos;
		newedge->mustNeg |= inedge->mustNeg;
		newedge->pseudoPos |= inedge->pseudoPos;
		//add new edge to both
		source->outEdges.add(newedge);
		node->inEdges.add(newedge);

		BooleanEdge be = solver->orderConstraint(graph->getOrder(), source->getID(), dstnode->getID());
		BooleanEdge benew = solver->orderConstraint(graph->getOrder(), source->getID(), node->getID());
		updateEdgePolarity(benew, be);
		if (solver->isTrue(benew))
			solver->replaceBooleanWithTrue(be);
		else if (solver->isFalse(benew))
			solver->replaceBooleanWithFalse(be);
		else
			solver->replaceBooleanWithBoolean(be, benew);
	}
	dstnode->inEdges.reset();
	delete inedgeit;

	/* Go through the outgoing edges from the new node */
	SetIteratorOrderEdge *outedgeit = dstnode->outEdges.iterator();
	while (outedgeit->hasNext()) {
		OrderEdge *outedge = outedgeit->next();
		OrderNode *sink = outedge->sink;
		//remove it from the sink node
		sink->inEdges.remove(outedge);
		//save the remapping that we did
		dor->remapEdge(dstnode->getID(), sink->getID(), node->getID(), sink->getID());

		//create the new edge
		OrderEdge *newedge = graph->getOrderEdgeFromOrderGraph(node, sink);
		//update the flags
		newedge->polPos |= outedge->polPos;
		newedge->polNeg |= outedge->polNeg;
		newedge->mustPos |= outedge->mustPos;
		newedge->mustNeg |= outedge->mustNeg;
		newedge->pseudoPos |= outedge->pseudoPos;
		//add new edge to both
		sink->inEdges.add(newedge);
		node->outEdges.add(newedge);

		BooleanEdge be = solver->orderConstraint(graph->getOrder(), dstnode->getID(), sink->getID());
		BooleanEdge benew = solver->orderConstraint(graph->getOrder(), node->getID(), sink->getID());
		updateEdgePolarity(benew, be);
		if (solver->isTrue(benew))
			solver->replaceBooleanWithTrue(be);
		else if (solver->isFalse(benew))
			solver->replaceBooleanWithFalse(be);
		else
			solver->replaceBooleanWithBoolean(be, benew);
	}
	dstnode->outEdges.reset();
	delete outedgeit;


	/* Mark destination as removed */
	dstnode->removed = true;
}
