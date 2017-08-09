
#include "orderencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"
#include "order.h"
#include "ordernode.h"


NodeInfo* allocNodeInfo(){
	NodeInfo* This = (NodeInfo*) ourmalloc(sizeof(NodeInfo));
	This->finishTime = 0;
	This->status = NOTVISITED;
	return This;
}

void deleteNodeInfo(NodeInfo* info){
	ourfree(info);
}

OrderEncoder* allocOrderEncoder(){
	OrderEncoder* This = (OrderEncoder*) ourmalloc(sizeof(OrderEncoder));
	initDefVectorOrderGraph( &This->graphs );
	return This;
}

void deleteOrderEncoder(OrderEncoder* This){
	uint size = getSizeVectorOrderGraph(&This->graphs);
	for(uint i=0; i<size; i++){
		deleteOrderGraph(getVectorOrderGraph(&This->graphs, i));
	}
	ourfree(This);
}

OrderEncoder* buildOrderGraphs(CSolver* This){
	uint size = getSizeVectorOrder(This->allOrders);
	OrderEncoder* oEncoder = allocOrderEncoder();
	for(uint i=0; i<size; i++){
		OrderGraph* orderGraph = allocOrderGraph();
		Order* order = getVectorOrder(This->allOrders, i);
		uint constrSize = getSizeVectorBoolean(&order->constraints);
		for(uint j=0; j<constrSize; j++){
			addOrderConstraintToOrderGraph(orderGraph, getVectorBoolean(&order->constraints, j));
		}
		pushVectorOrderGraph(&oEncoder->graphs, orderGraph);
	}
	return oEncoder;
}

void DFS(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo){
	uint timer=0;
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		NodeInfo* info= getNodeInfo(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			DFSNodeVisit(node, finishNodes, nodeToInfo, &timer, false);
			info->status = FINISHED;
			info->finishTime = timer;
			pushVectorOrderNode(finishNodes, node);
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSReverse(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo){
	uint timer=0;
	uint size = getSizeVectorOrderNode(finishNodes);
	for(int i=size-1; i>=0; i--){
		OrderNode* node = getVectorOrderNode(finishNodes, i);
		NodeInfo* info= getNodeInfo(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			DFSNodeVisit(node, NULL, nodeToInfo, &timer, true);
			info->status = FINISHED;
			info->finishTime = timer;
			pushVectorOrderNode(&graph->scc, node); 
		}
	}
}

void DFSNodeVisit(OrderNode* node, VectorOrderNode* finishNodes,
	HashTableNodeInfo* nodeToInfo, uint* timer, bool isReverse){
	(*timer)++;
	model_print("Timer in DFSNodeVisit:%u\n", *timer);
	HSIteratorOrderEdge* iterator = isReverse?iteratorOrderEdge(node->inEdges):iteratorOrderEdge(node->outEdges);
	while(hasNextOrderEdge(iterator)){
		OrderEdge* edge = nextOrderEdge(iterator);
		OrderNode* child = isReverse? edge->source: edge->sink;
		NodeInfo* childInfo = getNodeInfo(nodeToInfo, child);
		if(childInfo->status == NOTVISITED){
			childInfo->status = VISITED;
			DFSNodeVisit(child, finishNodes, nodeToInfo, timer, isReverse);
			childInfo->status = FINISHED;
			childInfo->finishTime = *timer;
			if(!isReverse)
				pushVectorOrderNode(finishNodes, child); 
		}
	}
	deleteIterOrderEdge(iterator);
}

void initializeNodeInfoSCC(OrderGraph* graph, HashTableNodeInfo* nodeToInfo){
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		putNodeInfo(nodeToInfo, nextOrderNode(iterator), allocNodeInfo());
	}
	deleteIterOrderNode(iterator);
}

void resetNodeInfoStatusSCC(OrderGraph* graph, HashTableNodeInfo* nodeToInfo){
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		NodeInfo* info= getNodeInfo(nodeToInfo, nextOrderNode(iterator));
		info->status = NOTVISITED;
	}
	deleteIterOrderNode(iterator);
}

void computeStronglyConnectedComponentGraph(OrderGraph* graph){
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	HashTableNodeInfo* nodeToInfo = allocHashTableNodeInfo(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	initializeNodeInfoSCC(graph, nodeToInfo);
	DFS(graph, &finishNodes, nodeToInfo);
	resetNodeInfoStatusSCC(graph, nodeToInfo);
	DFSReverse(graph, &finishNodes, nodeToInfo);
	deleteHashTableNodeInfo(nodeToInfo);
}

void removeMustBeTrueNodes(OrderGraph* graph){
	//TODO: Nodes that all the incoming/outgoing edges are MUST_BE_TRUE
}

void orderAnalysis(CSolver* solver){
	OrderEncoder* oEncoder = buildOrderGraphs(solver);
	uint size = getSizeVectorOrderGraph(&oEncoder->graphs);
	for(uint i=0; i<size; i++){
		OrderGraph* graph = getVectorOrderGraph(&oEncoder->graphs, i);
		removeMustBeTrueNodes(graph);
		computeStronglyConnectedComponentGraph(graph);
	}
}

