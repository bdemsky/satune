#include "orderencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"
#include "order.h"
#include "ordernode.h"


NodeInfo* allocNodeInfo() {
	NodeInfo* This = (NodeInfo*) ourmalloc(sizeof(NodeInfo));
	This->status = NOTVISITED;
	return This;
}

void deleteNodeInfo(NodeInfo* info) {
	ourfree(info);
}

OrderGraph* buildOrderGraph(Order *order) {
	OrderGraph* orderGraph = allocOrderGraph(order);
	uint constrSize = getSizeVectorBoolean(&order->constraints);
	for(uint j=0; j<constrSize; j++){
		addOrderConstraintToOrderGraph(orderGraph, getVectorBoolean(&order->constraints, j));
	}
	return orderGraph;
}

void DFS(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		NodeInfo* info= getNodeInfo(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			DFSNodeVisit(node, finishNodes, nodeToInfo, false);
			info->status = FINISHED;
			pushVectorOrderNode(finishNodes, node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSReverse(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo) {
	uint size = getSizeVectorOrderNode(finishNodes);
	for(int i=size-1; i>=0; i--){
		OrderNode* node = getVectorOrderNode(finishNodes, i);
		NodeInfo* info= getNodeInfo(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			DFSNodeVisit(node, NULL, nodeToInfo, true);
			info->status = FINISHED;
			pushVectorOrderNode(&graph->scc, node); 
		}
	}
}

void DFSNodeVisit(OrderNode* node, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo, bool isReverse) {
	HSIteratorOrderEdge* iterator = isReverse?iteratorOrderEdge(node->inEdges):iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		if (!edge->polPos && !edge->pseudoPos) //Ignore edges that do not have positive polarity
			continue;
		
		OrderNode* child = isReverse? edge->source: edge->sink;

		NodeInfo* childInfo = getNodeInfo(nodeToInfo, child);
		if(childInfo->status == NOTVISITED){
			childInfo->status = VISITED;
			DFSNodeVisit(child, finishNodes, nodeToInfo, isReverse);
			childInfo->status = FINISHED;
			if(!isReverse)
				pushVectorOrderNode(finishNodes, child); 
		}
	}
	deleteIterOrderEdge(iterator);
}

void initializeNodeInfoSCC(OrderGraph* graph, HashTableNodeInfo* nodeToInfo) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		putNodeInfo(nodeToInfo, nextOrderNode(iterator), allocNodeInfo());
	}
	deleteIterOrderNode(iterator);
}

void resetNodeInfoStatusSCC(OrderGraph* graph, HashTableNodeInfo* nodeToInfo) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		NodeInfo* info= getNodeInfo(nodeToInfo, nextOrderNode(iterator));
		info->status = NOTVISITED;
	}
	deleteIterOrderNode(iterator);
}

void computeStronglyConnectedComponentGraph(OrderGraph* graph) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	HashTableNodeInfo* nodeToInfo = allocHashTableNodeInfo(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	initializeNodeInfoSCC(graph, nodeToInfo);
	DFS(graph, &finishNodes, nodeToInfo);
	resetNodeInfoStatusSCC(graph, nodeToInfo);
	DFSReverse(graph, &finishNodes, nodeToInfo);
	deleteHashTableNodeInfo(nodeToInfo);
	deleteVectorArrayOrderNode(&finishNodes);
}

void removeMustBeTrueNodes(OrderGraph* graph) {
	//TODO: Nodes that all the incoming/outgoing edges are MUST_BE_TRUE
}

void DFSPseudoNodeVisit(OrderGraph *graph, OrderNode* node, HashTableNodeInfo* nodeToInfo) {
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(node->inEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* inEdge = nextOrderEdge(iterator);
		if (inEdge->polNeg) {
			OrderNode* src = inEdge->source;
			NodeInfo* srcInfo = getNodeInfo(nodeToInfo, src);
			if (srcInfo->status==VISITED) {
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
		NodeInfo* childInfo = getNodeInfo(nodeToInfo, child);
		if(childInfo->status == NOTVISITED){
			childInfo->status = VISITED;
			DFSPseudoNodeVisit(graph, child, nodeToInfo);
			childInfo->status = FINISHED;
		}
	}
	deleteIterOrderEdge(iterator);
}

void completePartialOrderGraph(OrderGraph* graph) {
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	HashTableNodeInfo* nodeToInfo = allocHashTableNodeInfo(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	initializeNodeInfoSCC(graph, nodeToInfo);
	DFS(graph, &finishNodes, nodeToInfo);
	resetNodeInfoStatusSCC(graph, nodeToInfo);

	uint size = getSizeVectorOrderNode(&finishNodes);
	for(int i=size-1; i>=0; i--){
		OrderNode* node = getVectorOrderNode(&finishNodes, i);
		NodeInfo* info= getNodeInfo(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			DFSPseudoNodeVisit(graph, node, nodeToInfo);
			info->status = FINISHED;
		}
	}
	
	deleteHashTableNodeInfo(nodeToInfo);
	deleteVectorArrayOrderNode(&finishNodes);
}

void DFSMust(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo) {
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		NodeInfo* info= getNodeInfo(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			DFSMustNodeVisit(node, finishNodes, nodeToInfo, false);
			info->status = FINISHED;
			pushVectorOrderNode(finishNodes, node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSMustNodeVisit(OrderNode* node, VectorOrderNode* finishNodes,
											HashTableNodeInfo* nodeToInfo, bool clearBackEdges) {
	HSIteratorOrderEdge* iterator = iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		OrderNode* child = edge->sink;
		NodeInfo* childInfo = getNodeInfo(nodeToInfo, child);
		
		if (clearBackEdges && childInfo->status==VISITED) {
			//We have a backedge, so note that this edge must be negative
			edge->mustNeg = true;
		}

		if (!edge->mustPos) //Ignore edges that are not must Positive edges
			continue;

		if(childInfo->status == NOTVISITED){
			childInfo->status = VISITED;
			DFSMustNodeVisit(child, finishNodes, nodeToInfo, clearBackEdges);
			childInfo->status = FINISHED;
			pushVectorOrderNode(finishNodes, child); 
		}
	}
	deleteIterOrderEdge(iterator);
}

void DFSClearContradictions(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo) {
	uint size=getSizeVectorOrderNode(finishNodes);
	for(int i=size-1; i>=0; i--){
		OrderNode* node=getVectorOrderNode(finishNodes, i);
		NodeInfo* info=getNodeInfo(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			DFSMustNodeVisit(node, NULL, nodeToInfo, true);
			info->status = FINISHED;
		}
	}
}

/* This function finds edges that would form a cycle with must edges
	 and forces them to be mustNeg. */

void reachMustAnalysis(OrderGraph *graph) {
	HashTableNodeInfo* nodeToInfo = allocHashTableNodeInfo(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	//Topologically sort the mustPos edge graph
	DFSMust(graph, &finishNodes, nodeToInfo);
	resetNodeInfoStatusSCC(graph, nodeToInfo);

	//Find any backwards edges that complete cycles and force them to be mustNeg
	DFSClearContradictions(graph, &finishNodes, nodeToInfo);
	deleteVectorArrayOrderNode(&finishNodes);
	deleteHashTableNodeInfo(nodeToInfo);
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

		//This analysis is completely optional
		removeMustBeTrueNodes(graph);

		
		computeStronglyConnectedComponentGraph(graph);
		deleteOrderGraph(graph);
	}
}
