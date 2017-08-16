#include "orderencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"
#include "order.h"
#include "ordernode.h"


OrderGraph* buildOrderGraph(Order *order) {
	OrderGraph* orderGraph = allocOrderGraph(order);
	uint constrSize = getSizeVectorBooleanOrder(&order->constraints);
	for(uint j=0; j<constrSize; j++){
		addOrderConstraintToOrderGraph(orderGraph, getVectorBooleanOrder(&order->constraints, j));
	}
	return orderGraph;
}

void DFS(OrderGraph* graph, VectorOrderNode* finishNodes) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		if(node->status == NOTVISITED){
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, 0);
			node->status = FINISHED;
			pushVectorOrderNode(finishNodes, node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSReverse(OrderGraph* graph, VectorOrderNode* finishNodes) {
	uint size = getSizeVectorOrderNode(finishNodes);
	uint sccNum=1;
	for(int i=size-1; i>=0; i--){
		OrderNode* node = getVectorOrderNode(finishNodes, i);
		if(node->status == NOTVISITED){
			node->status = VISITED;
			DFSNodeVisit(node, NULL, true, sccNum);
			node->sccNum = sccNum;
			node->status = FINISHED;
			sccNum++;
		}
	}
}

void DFSNodeVisit(OrderNode* node, VectorOrderNode* finishNodes, bool isReverse, uint sccNum) {
	HSIteratorOrderEdge* iterator = isReverse?iteratorOrderEdge(node->inEdges):iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		if (!edge->polPos && !edge->pseudoPos) //Ignore edges that do not have positive polarity
			continue;
		
		OrderNode* child = isReverse? edge->source: edge->sink;

		if(child->status == NOTVISITED) {
			child->status = VISITED;
			DFSNodeVisit(child, finishNodes, isReverse, sccNum);
			child->status = FINISHED;
			if(!isReverse)
				pushVectorOrderNode(finishNodes, child); 
			else
				child->sccNum = sccNum;
		}
	}
	deleteIterOrderEdge(iterator);
}

void resetNodeInfoStatusSCC(OrderGraph* graph) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		nextOrderNode(iterator)->status = NOTVISITED;
	}
	deleteIterOrderNode(iterator);
}

void computeStronglyConnectedComponentGraph(OrderGraph* graph) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	DFSReverse(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	deleteVectorArrayOrderNode(&finishNodes);
}

void removeMustBeTrueNodes(OrderGraph* graph) {
	//TODO: Nodes that all the incoming/outgoing edges are MUST_BE_TRUE
}

void DFSPseudoNodeVisit(OrderGraph *graph, OrderNode* node) {
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(node->inEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* inEdge = nextOrderEdge(iterator);
		if (inEdge->polNeg) {
			OrderNode* src = inEdge->source;
			if (src->status==VISITED) {
				//Make a pseudoEdge to point backwards
				OrderEdge * newedge = getOrderEdgeFromOrderGraph(graph, inEdge->sink, inEdge->source);
				newedge->pseudoPos = true;
			}
		}
	}
	deleteIterOrderEdge(iterator);
	iterator = iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		if (!edge->polPos) //Ignore edges that do not have positive polarity
			continue;
		
		OrderNode* child = edge->sink;
		if(child->status == NOTVISITED){
			child->status = VISITED;
			DFSPseudoNodeVisit(graph, child);
			child->status = FINISHED;
		}
	}
	deleteIterOrderEdge(iterator);
}

void completePartialOrderGraph(OrderGraph* graph) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);

	uint size = getSizeVectorOrderNode(&finishNodes);
	for(int i=size-1; i>=0; i--){
		OrderNode* node = getVectorOrderNode(&finishNodes, i);
		if(node->status == NOTVISITED){
			node->status = VISITED;
			DFSPseudoNodeVisit(graph, node);
			node->status = FINISHED;
		}
	}

	resetNodeInfoStatusSCC(graph);
	deleteVectorArrayOrderNode(&finishNodes);
}

void DFSMust(OrderGraph* graph, VectorOrderNode* finishNodes) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		if(node->status == NOTVISITED){
			node->status = VISITED;
			DFSMustNodeVisit(node, finishNodes, false);
			node->status = FINISHED;
			pushVectorOrderNode(finishNodes, node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSMustNodeVisit(OrderNode* node, VectorOrderNode* finishNodes, bool clearBackEdges) {
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		OrderNode* child = edge->sink;
		
		if (clearBackEdges && child->status==VISITED) {
			//We have a backedge, so note that this edge must be negative
			edge->mustNeg = true;
		}

		if (!edge->mustPos) //Ignore edges that are not must Positive edges
			continue;

		if(child->status == NOTVISITED){
			child->status = VISITED;
			DFSMustNodeVisit(child, finishNodes, clearBackEdges);
			child->status = FINISHED;
			pushVectorOrderNode(finishNodes, child); 
		}
	}
	deleteIterOrderEdge(iterator);
}

void DFSClearContradictions(OrderGraph* graph, VectorOrderNode* finishNodes) {
	uint size=getSizeVectorOrderNode(finishNodes);
	for(int i=size-1; i>=0; i--){
		OrderNode* node=getVectorOrderNode(finishNodes, i);
		if(node->status == NOTVISITED){
			node->status = VISITED;
			DFSMustNodeVisit(node, NULL, true);
			node->status = FINISHED;
		}
	}
}

/* This function finds edges that would form a cycle with must edges
	 and forces them to be mustNeg. */

void reachMustAnalysis(OrderGraph *graph) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	//Topologically sort the mustPos edge graph
	DFSMust(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);

	//Find any backwards edges that complete cycles and force them to be mustNeg
	DFSClearContradictions(graph, &finishNodes);
	deleteVectorArrayOrderNode(&finishNodes);
	resetNodeInfoStatusSCC(graph);
}

/* This function finds edges that must be positive and forces the
	 inverse edge to be negative (and clears its positive polarity if it
	 had one). */

void localMustAnalysisTotal(OrderGraph *graph) {
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(graph->edges);
	while(hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (edge -> mustPos) {
			OrderEdge *invEdge=getInverseOrderEdge(graph, edge);
			if (invEdge!= NULL && !invEdge -> mustPos && invEdge->polPos) {
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
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(graph->edges);
	while(hasNextOrderEdge(iterator)) {
		OrderEdge *edge = nextOrderEdge(iterator);
		if (edge -> mustPos) {
			if (edge->polNeg && !edge->mustNeg) {
				edge->polNeg = false;
			}
			OrderEdge *invEdge=getInverseOrderEdge(graph, edge);
			if (invEdge != NULL && !invEdge -> mustPos) {
				invEdge->polPos = false;
			}
			invEdge->mustNeg = true;
		}
		if (edge->mustNeg && !edge->mustPos) {
			edge->polPos = false;
		}
	}
	deleteIterOrderEdge(iterator);
}

void decomposeOrder(Order *order, OrderGraph *graph) {
	uint size=getSizeVectorBooleanOrder(&order->constraints);
	for(uint i=0;i<size;i++) {
	}
}

void orderAnalysis(CSolver* This) {
	uint size = getSizeVectorOrder(This->allOrders);
	for(uint i=0; i<size; i++){
		Order* order = getVectorOrder(This->allOrders, i);
		OrderGraph* graph = buildOrderGraph(order);
		if (order->type==PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			completePartialOrderGraph(graph);
		}

		//This analysis is completely optional
		reachMustAnalysis(graph);
		
		//This pair of analysis is also optional
		if (order->type==PARTIAL) {
			localMustAnalysisPartial(graph);
		} else {
			localMustAnalysisTotal(graph);
		}

		//This optimization is completely optional
		removeMustBeTrueNodes(graph);

		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);

		decomposeOrder(order, graph);
			
		deleteOrderGraph(graph);
	}
}
