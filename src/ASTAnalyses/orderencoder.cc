#include "orderencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"
#include "order.h"
#include "ordernode.h"
#include "rewriter.h"
#include "mutableset.h"
#include "tunable.h"

void DFS(OrderGraph *graph, Vector<OrderNode *> *finishNodes) {
	HSIteratorOrderNode *iterator = iteratorOrderNode(graph->nodes);
	while (hasNextOrderNode(iterator)) {
		OrderNode *node = nextOrderNode(iterator);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, false, 0);
			node->status = FINISHED;
			finishNodes->push(node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSReverse(OrderGraph *graph, Vector<OrderNode *> *finishNodes) {
	uint size = finishNodes->getSize();
	uint sccNum = 1;
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes->get(i);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, NULL, true, false, sccNum);
			node->sccNum = sccNum;
			node->status = FINISHED;
			sccNum++;
		}
	}
}

void DFSNodeVisit(OrderNode *node, Vector<OrderNode *> *finishNodes, bool isReverse, bool mustvisit, uint sccNum) {
	HSIteratorOrderEdge *iterator = isReverse ? iteratorOrderEdge(node->inEdges) : iteratorOrderEdge(node->outEdges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (mustvisit) {
			if (!edge->mustPos)
				continue;
		} else
			if (!edge->polPos && !edge->pseudoPos)//Ignore edges that do not have positive polarity
				continue;

		OrderNode *child = isReverse ? edge->source : edge->sink;

		if (child->status == NOTVISITED) {
			child->status = VISITED;
			DFSNodeVisit(child, finishNodes, isReverse, mustvisit, sccNum);
			child->status = FINISHED;
			if (finishNodes != NULL)
				finishNodes->push(child);
			if (isReverse)
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
	Vector<OrderNode *> finishNodes;
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	DFSReverse(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
}

bool isMustBeTrueNode(OrderNode* node){
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(node->inEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		if(!edge->mustPos)
			return false;
	}
	deleteIterOrderEdge(iterator);
	iterator = iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		if(!edge->mustPos)
			return false;
	}
	deleteIterOrderEdge(iterator);
	return true;
}

void bypassMustBeTrueNode(CSolver *This, OrderGraph* graph, OrderNode* node){
	HSIteratorOrderEdge* iterin = iteratorOrderEdge(node->inEdges);
	while(hasNextOrderEdge(iterin)){
		OrderEdge* inEdge = nextOrderEdge(iterin);
		OrderNode* srcNode = inEdge->source;
		removeHashSetOrderEdge(srcNode->outEdges, inEdge);
		HSIteratorOrderEdge* iterout = iteratorOrderEdge(node->outEdges);
		while(hasNextOrderEdge(iterout)){
			OrderEdge* outEdge = nextOrderEdge(iterout);
			OrderNode* sinkNode = outEdge->sink;
			removeHashSetOrderEdge(sinkNode->inEdges, outEdge);
			//Adding new edge to new sink and src nodes ...
			OrderEdge *newEdge =getOrderEdgeFromOrderGraph(graph, srcNode, sinkNode);
			newEdge->mustPos = true;
			newEdge->polPos = true;
			if (newEdge->mustNeg)
				This->unsat = true;
			addHashSetOrderEdge(srcNode->outEdges, newEdge);
			addHashSetOrderEdge(sinkNode->inEdges, newEdge);
		}
		deleteIterOrderEdge(iterout);
	}
	deleteIterOrderEdge(iterin);
}

void removeMustBeTrueNodes(CSolver *This, OrderGraph *graph) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		if(isMustBeTrueNode(node)){
			bypassMustBeTrueNode(This,graph, node);
		}
	}
	deleteIterOrderNode(iterator);
}

/** This function computes a source set for every nodes, the set of
		nodes that can reach that node via pospolarity edges.  It then
		looks for negative polarity edges from nodes in the the source set
		to determine whether we need to generate pseudoPos edges. */

void completePartialOrderGraph(OrderGraph *graph) {
	Vector<OrderNode *> finishNodes;
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	HashTableNodeToNodeSet *table = allocHashTableNodeToNodeSet(128, 0.25);

	Vector<OrderNode *> sccNodes;
	
	uint size = finishNodes.getSize();
	uint sccNum = 1;
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes.get(i);
		HashSetOrderNode *sources = allocHashSetOrderNode(4, 0.25);
		putNodeToNodeSet(table, node, sources);
		
		if (node->status == NOTVISITED) {
			//Need to do reverse traversal here...
			node->status = VISITED;
			DFSNodeVisit(node, &sccNodes, true, false, sccNum);
			node->status = FINISHED;
			node->sccNum = sccNum;
			sccNum++;
			sccNodes.push(node);

			//Compute in set for entire SCC
			uint rSize = sccNodes.getSize();
			for (uint j = 0; j < rSize; j++) {
				OrderNode *rnode = sccNodes.get(j);
				//Compute source sets
				HSIteratorOrderEdge *iterator = iteratorOrderEdge(rnode->inEdges);
				while (hasNextOrderEdge(iterator)) {
					OrderEdge *edge = nextOrderEdge(iterator);
					OrderNode *parent = edge->source;
					if (edge->polPos) {
						addHashSetOrderNode(sources, parent);
						HashSetOrderNode *parent_srcs = (HashSetOrderNode *)getNodeToNodeSet(table, parent);
						addAllHashSetOrderNode(sources, parent_srcs);
					}
				}
				deleteIterOrderEdge(iterator);
			}
			for (uint j=0; j < rSize; j++) {
				//Copy in set of entire SCC
				OrderNode *rnode = sccNodes.get(j);
				HashSetOrderNode * set = (j==0) ? sources : copyHashSetOrderNode(sources);
				putNodeToNodeSet(table, rnode, set);

				//Use source sets to compute pseudoPos edges
				HSIteratorOrderEdge *iterator = iteratorOrderEdge(rnode->inEdges);
				while (hasNextOrderEdge(iterator)) {
					OrderEdge *edge = nextOrderEdge(iterator);
					OrderNode *parent = edge->source;
					ASSERT(parent != rnode);
					if (edge->polNeg && parent->sccNum != rnode->sccNum &&
							containsHashSetOrderNode(sources, parent)) {
						OrderEdge *newedge = getOrderEdgeFromOrderGraph(graph, rnode, parent);
						newedge->pseudoPos = true;
					}
				}
				deleteIterOrderEdge(iterator);
			}
			
			sccNodes.clear();
		}
	}

	resetAndDeleteHashTableNodeToNodeSet(table);
	deleteHashTableNodeToNodeSet(table);
	resetNodeInfoStatusSCC(graph);
}

void DFSMust(OrderGraph *graph, Vector<OrderNode *> *finishNodes) {
	HSIteratorOrderNode *iterator = iteratorOrderNode(graph->nodes);
	while (hasNextOrderNode(iterator)) {
		OrderNode *node = nextOrderNode(iterator);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, true, 0);
			node->status = FINISHED;
			finishNodes->push(node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSClearContradictions(CSolver *solver, OrderGraph *graph, Vector<OrderNode *> *finishNodes, bool computeTransitiveClosure) {
	uint size = finishNodes->getSize();
	HashTableNodeToNodeSet *table = allocHashTableNodeToNodeSet(128, 0.25);

	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes->get(i);
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
				if (newedge->mustNeg)
					solver->unsat = true;
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
					edge->polPos = true;
					if (edge->mustNeg)
						solver->unsat = true;
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
					edge->polNeg = true;
					if (edge->mustPos)
						solver->unsat = true;
				}
			}
			deleteIterOrderEdge(iterator);
		}
	}

	resetAndDeleteHashTableNodeToNodeSet(table);
	deleteHashTableNodeToNodeSet(table);
}

/* This function finds edges that would form a cycle with must edges
   and forces them to be mustNeg.  It also decides whether an edge
   must be true because of transitivity from other must be true
   edges. */

void reachMustAnalysis(CSolver * solver, OrderGraph *graph, bool computeTransitiveClosure) {
	Vector<OrderNode *> finishNodes;
	//Topologically sort the mustPos edge graph
	DFSMust(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);

	//Find any backwards edges that complete cycles and force them to be mustNeg
	DFSClearContradictions(solver, graph, &finishNodes, computeTransitiveClosure);
}

/* This function finds edges that must be positive and forces the
   inverse edge to be negative (and clears its positive polarity if it
   had one). */

void localMustAnalysisTotal(CSolver *solver, OrderGraph *graph) {
	HSIteratorOrderEdge *iterator = iteratorOrderEdge(graph->edges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (edge->mustPos) {
			OrderEdge *invEdge = getInverseOrderEdge(graph, edge);
			if (invEdge != NULL) {
				if (!invEdge->mustPos) {
					invEdge->polPos = false;
				} else {
					solver->unsat = true;
				}
				invEdge->mustNeg = true;
				invEdge->polNeg = true;
			}
		}
	}
	deleteIterOrderEdge(iterator);
}

/** This finds edges that must be positive and forces the inverse edge
    to be negative.  It also clears the negative flag of this edge.
    It also finds edges that must be negative and clears the positive
    polarity. */

void localMustAnalysisPartial(CSolver *solver, OrderGraph *graph) {
	HSIteratorOrderEdge *iterator = iteratorOrderEdge(graph->edges);
	while (hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (edge->mustPos) {
			if (!edge->mustNeg) {
				edge->polNeg = false;
			} else
				solver->unsat = true;

			OrderEdge *invEdge = getInverseOrderEdge(graph, edge);
			if (invEdge != NULL) {
				if (!invEdge->mustPos)
					invEdge->polPos = false;
				else
					solver->unsat = true;
				invEdge->mustNeg = true;
				invEdge->polNeg = true;
			}
		}
		if (edge->mustNeg && !edge->mustPos) {
			edge->polPos = false;
		}
	}
	deleteIterOrderEdge(iterator);
}

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

void orderAnalysis(CSolver *This) {
	uint size = This->allOrders.getSize();
	for (uint i = 0; i < size; i++) {
		Order *order = This->allOrders.get(i);
		bool doDecompose=GETVARTUNABLE(This->tuner, order->type, DECOMPOSEORDER, &onoff);
		if (!doDecompose)
			continue;
		
		OrderGraph *graph = buildOrderGraph(order);
		if (order->type == PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			completePartialOrderGraph(graph);
		}


		bool mustReachGlobal=GETVARTUNABLE(This->tuner, order->type, MUSTREACHGLOBAL, &onoff);

		if (mustReachGlobal)
			reachMustAnalysis(This, graph, false);

		bool mustReachLocal=GETVARTUNABLE(This->tuner, order->type, MUSTREACHLOCAL, &onoff);
		
		if (mustReachLocal) {
			//This pair of analysis is also optional
			if (order->type == PARTIAL) {
				localMustAnalysisPartial(This, graph);
			} else {
				localMustAnalysisTotal(This, graph);
			}
		}

		bool mustReachPrune=GETVARTUNABLE(This->tuner, order->type, MUSTREACHPRUNE, &onoff);
		
		if (mustReachPrune)
			removeMustBeTrueNodes(This, graph);
		
		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);
		
		decomposeOrder(This, order, graph);
		
		deleteOrderGraph(graph);
	}
}
