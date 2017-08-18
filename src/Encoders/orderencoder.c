#include "orderencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"
#include "order.h"
#include "ordernode.h"
#include "rewriter.h"
#include "mutableset.h"

OrderGraph *buildOrderGraph(Order *order) {
	OrderGraph *orderGraph = allocOrderGraph(order);
	uint constrSize = getSizeVectorBooleanOrder(&order->constraints);
	for (uint j = 0; j < constrSize; j++) {
		addOrderConstraintToOrderGraph(orderGraph, getVectorBooleanOrder(&order->constraints, j));
	}
	return orderGraph;
}

void DFS(OrderGraph *graph, VectorOrderNode *finishNodes) {
	HSIteratorOrderNode *iterator = iteratorOrderNode(graph->nodes);
	while (hasNextOrderNode(iterator)) {
		OrderNode *node = nextOrderNode(iterator);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, 0);
			node->status = FINISHED;
			pushVectorOrderNode(finishNodes, node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSReverse(OrderGraph *graph, VectorOrderNode *finishNodes) {
	uint size = getSizeVectorOrderNode(finishNodes);
	uint sccNum = 1;
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = getVectorOrderNode(finishNodes, i);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, NULL, true, sccNum);
			node->sccNum = sccNum;
			node->status = FINISHED;
			sccNum++;
		}
	}
}

void DFSNodeVisit(OrderNode *node, VectorOrderNode *finishNodes, bool isReverse, uint sccNum) {
	HSIteratorOrderEdge *iterator = isReverse ? iteratorOrderEdge(node->inEdges) : iteratorOrderEdge(node->outEdges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (!edge->polPos && !edge->pseudoPos)//Ignore edges that do not have positive polarity
			continue;

		OrderNode *child = isReverse ? edge->source : edge->sink;

		if (child->status == NOTVISITED) {
			child->status = VISITED;
			DFSNodeVisit(child, finishNodes, isReverse, sccNum);
			child->status = FINISHED;
			if (!isReverse)
				pushVectorOrderNode(finishNodes, child);
			else
				child->sccNum = sccNum;
		}
	}
	deleteIterOrderEdge(iterator);
}

void resetNodeInfoStatusSCC(OrderGraph *graph) {
	HSIteratorOrderNode *iterator = iteratorOrderNode(graph->nodes);
	while (hasNextOrderNode(iterator)) {
		nextOrderNode(iterator)->status = NOTVISITED;
	}
	deleteIterOrderNode(iterator);
}

void computeStronglyConnectedComponentGraph(OrderGraph *graph) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(&finishNodes);
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	DFSReverse(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	deleteVectorArrayOrderNode(&finishNodes);
}

void removeMustBeTrueNodes(OrderGraph *graph) {
	//TODO: Nodes that all the incoming/outgoing edges are MUST_BE_TRUE
}

void DFSPseudoNodeVisit(OrderGraph *graph, OrderNode *node) {
	HSIteratorOrderEdge *iterator = iteratorOrderEdge(node->inEdges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *inEdge = nextOrderEdge(iterator);
		if (inEdge->polNeg) {
			OrderNode *src = inEdge->source;
			if (src->status == VISITED) {
				//Make a pseudoEdge to point backwards
				OrderEdge *newedge = getOrderEdgeFromOrderGraph(graph, inEdge->sink, inEdge->source);
				newedge->pseudoPos = true;
			}
		}
	}
	deleteIterOrderEdge(iterator);
	iterator = iteratorOrderEdge(node->outEdges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (!edge->polPos)//Ignore edges that do not have positive polarity
			continue;

		OrderNode *child = edge->sink;
		if (child->status == NOTVISITED) {
			child->status = VISITED;
			DFSPseudoNodeVisit(graph, child);
			child->status = FINISHED;
		}
	}
	deleteIterOrderEdge(iterator);
}

void completePartialOrderGraph(OrderGraph *graph) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(&finishNodes);
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);

	uint size = getSizeVectorOrderNode(&finishNodes);
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = getVectorOrderNode(&finishNodes, i);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSPseudoNodeVisit(graph, node);
			node->status = FINISHED;
		}
	}

	resetNodeInfoStatusSCC(graph);
	deleteVectorArrayOrderNode(&finishNodes);
}

void DFSMust(OrderGraph *graph, VectorOrderNode *finishNodes) {
	HSIteratorOrderNode *iterator = iteratorOrderNode(graph->nodes);
	while (hasNextOrderNode(iterator)) {
		OrderNode *node = nextOrderNode(iterator);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSMustNodeVisit(node, finishNodes);
			node->status = FINISHED;
			pushVectorOrderNode(finishNodes, node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSMustNodeVisit(OrderNode *node, VectorOrderNode *finishNodes) {
	HSIteratorOrderEdge *iterator = iteratorOrderEdge(node->outEdges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		OrderNode *child = edge->sink;

		if (!edge->mustPos)	//Ignore edges that are not must Positive edges
			continue;

		if (child->status == NOTVISITED) {
			child->status = VISITED;
			DFSMustNodeVisit(child, finishNodes);
			child->status = FINISHED;
			pushVectorOrderNode(finishNodes, child);
		}
	}
	deleteIterOrderEdge(iterator);
}


void DFSClearContradictions(OrderGraph *graph, VectorOrderNode *finishNodes, bool computeTransitiveClosure) {
	uint size = getSizeVectorOrderNode(finishNodes);
	HashTableNodeToNodeSet *table = allocHashTableNodeToNodeSet(128, 0.25);

	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = getVectorOrderNode(finishNodes, i);
		HashSetOrderNode *sources = allocHashSetOrderNode(4, 0.25);
		putNodeToNodeSet(table, node, sources);

		{
			//Compute source sets
			HSIteratorOrderEdge *iterator = iteratorOrderEdge(node->inEdges);
			while (hasNextOrderEdge(iterator)) {
				OrderEdge *edge = nextOrderEdge(iterator);
				OrderNode *parent = edge->source;
				if (edge->mustPos) {
					addHashSetOrderNode(sources, parent);
					HashSetOrderNode *parent_srcs = (HashSetOrderNode *)getNodeToNodeSet(table, parent);
					addAllHashSetOrderNode(sources, parent_srcs);
				}
			}
			deleteIterOrderEdge(iterator);
		}
		if (computeTransitiveClosure) {
			//Compute full transitive closure for nodes
			HSIteratorOrderNode *srciterator = iteratorOrderNode(sources);
			while (hasNextOrderNode(srciterator)) {
				OrderNode *srcnode = nextOrderNode(srciterator);
				OrderEdge *newedge = getOrderEdgeFromOrderGraph(graph, srcnode, node);
				newedge->mustPos = true;
				newedge->polPos = true;
				addHashSetOrderEdge(srcnode->outEdges,newedge);
				addHashSetOrderEdge(node->inEdges,newedge);
			}
			deleteIterOrderNode(srciterator);
		}
		{
			//Use source sets to compute mustPos edges
			HSIteratorOrderEdge *iterator = iteratorOrderEdge(node->inEdges);
			while (hasNextOrderEdge(iterator)) {
				OrderEdge *edge = nextOrderEdge(iterator);
				OrderNode *parent = edge->source;
				if (!edge->mustPos && containsHashSetOrderNode(sources, parent)) {
					edge->mustPos = true;
				}
			}
			deleteIterOrderEdge(iterator);
		}
		{
			//Use source sets to compute mustNeg for edges that would introduce cycle if true
			HSIteratorOrderEdge *iterator = iteratorOrderEdge(node->outEdges);
			while (hasNextOrderEdge(iterator)) {
				OrderEdge *edge = nextOrderEdge(iterator);
				OrderNode *child = edge->sink;
				if (!edge->mustNeg && containsHashSetOrderNode(sources, child)) {
					edge->mustNeg = true;
				}
			}
			deleteIterOrderEdge(iterator);
		}
	}

	resetAndDeleteHashTableNodeToNodeSet(table);
}

/* This function finds edges that would form a cycle with must edges
   and forces them to be mustNeg.  It also decides whether an edge
   must be true because of transitivity from other must be true
   edges. */

void reachMustAnalysis(OrderGraph *graph, bool computeTransitiveClosure) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(&finishNodes);
	//Topologically sort the mustPos edge graph
	DFSMust(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);

	//Find any backwards edges that complete cycles and force them to be mustNeg
	DFSClearContradictions(graph, &finishNodes, computeTransitiveClosure);
	deleteVectorArrayOrderNode(&finishNodes);
	resetNodeInfoStatusSCC(graph);
}

/* This function finds edges that must be positive and forces the
   inverse edge to be negative (and clears its positive polarity if it
   had one). */

void localMustAnalysisTotal(OrderGraph *graph) {
	HSIteratorOrderEdge *iterator = iteratorOrderEdge(graph->edges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (edge->mustPos) {
			OrderEdge *invEdge = getInverseOrderEdge(graph, edge);
			if (invEdge != NULL && !invEdge->mustPos && invEdge->polPos) {
				invEdge->polPos = false;
			}
			invEdge->mustNeg = true;
		}
	}
	deleteIterOrderEdge(iterator);
}

/** This finds edges that must be positive and forces the inverse edge
    to be negative.  It also clears the negative flag of this edge.
    It also finds edges that must be negative and clears the positive
    polarity. */

void localMustAnalysisPartial(OrderGraph *graph) {
	HSIteratorOrderEdge *iterator = iteratorOrderEdge(graph->edges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (edge->mustPos) {
			if (edge->polNeg && !edge->mustNeg) {
				edge->polNeg = false;
			}
			OrderEdge *invEdge = getInverseOrderEdge(graph, edge);
			if (invEdge != NULL) {
				if (!invEdge->mustPos)
					invEdge->polPos = false;
				invEdge->mustNeg = true;
			}
		}
		if (edge->mustNeg && !edge->mustPos) {
			edge->polPos = false;
		}
	}
	deleteIterOrderEdge(iterator);
}

void decomposeOrder(CSolver *This, Order *order, OrderGraph *graph) {
	VectorOrder ordervec;
	initDefVectorOrder(&ordervec);
	uint size = getSizeVectorBooleanOrder(&order->constraints);
	for (uint i = 0; i < size; i++) {
		BooleanOrder *orderconstraint = getVectorBooleanOrder(&order->constraints, i);
		OrderNode *from = getOrderNodeFromOrderGraph(graph, orderconstraint->first);
		OrderNode *to = getOrderNodeFromOrderGraph(graph, orderconstraint->second);
		OrderEdge *edge = getOrderEdgeFromOrderGraph(graph, from, to);
		if (from->sccNum < to->sccNum) {
			//replace with true
			replaceBooleanWithTrue(This, (Boolean *)orderconstraint);
		} else if (to->sccNum < from->sccNum) {
			//replace with false
			replaceBooleanWithFalse(This, (Boolean *)orderconstraint);
		} else {
			//Build new order and change constraint's order
			Order *neworder = NULL;
			if (getSizeVectorOrder(&ordervec) > from->sccNum)
				neworder = getVectorOrder(&ordervec, from->sccNum);
			if (neworder == NULL) {
				Set *set = (Set *) allocMutableSet(order->set->type);
				neworder = allocOrder(order->type, set);
				pushVectorOrder(This->allOrders, neworder);
				setExpandVectorOrder(&ordervec, from->sccNum, neworder);
			}
			if (from->status != ADDEDTOSET) {
				from->status = ADDEDTOSET;
				addElementMSet((MutableSet *)neworder->set, from->id);
			}
			if (to->status != ADDEDTOSET) {
				to->status = ADDEDTOSET;
				addElementMSet((MutableSet *)neworder->set, to->id);
			}
			orderconstraint->order = neworder;
			addOrderConstraint(neworder, orderconstraint);
		}
	}
	deleteVectorArrayOrder(&ordervec);
}

void orderAnalysis(CSolver *This) {
	uint size = getSizeVectorOrder(This->allOrders);
	for (uint i = 0; i < size; i++) {
		Order *order = getVectorOrder(This->allOrders, i);
		OrderGraph *graph = buildOrderGraph(order);
		if (order->type == PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			completePartialOrderGraph(graph);
		}

		//This analysis is completely optional
		reachMustAnalysis(graph, false);

		//This pair of analysis is also optional
		if (order->type == PARTIAL) {
			localMustAnalysisPartial(graph);
		} else {
			localMustAnalysisTotal(graph);
		}

		//This optimization is completely optional
		removeMustBeTrueNodes(graph);

		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);

		decomposeOrder(This, order, graph);

		deleteOrderGraph(graph);
	}
}
