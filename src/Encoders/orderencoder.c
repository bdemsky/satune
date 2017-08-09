
#include "orderencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"
#include "order.h"


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

void DFS(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNode* nodeToInfo, bool isReverse){
	uint timer=0;
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		OrderNode* node = nextOrderNode(iterator);
		NodeInfo* info= getNode(nodeToInfo, node);
		if(info->status == NOTVISITED){
			info->status = VISITED;
			//TODO ...
		}
	}
	deleteIterOrderNode(iterator);
}

void DFSNodeVisit(OrderNode* node, VectorOrderNode* finishNodes, HashTableNode* nodeToInfo, uint* timer, bool isReverse){
	NodeInfo* info= getNode(nodeToInfo, node);
}

void initializeNodeInfoSCC(OrderGraph* graph, HashTableNode* nodeToInfo){
	HSIteratorOrderNode* iterator = iteratorOrderNode(graph->nodes);
	while(hasNextOrderNode(iterator)){
		putNode(nodeToInfo, nextOrderNode(iterator), allocNodeInfo());
	}
	deleteIterOrderNode(iterator);
}

void computeStronglyConnectedComponentGraph(OrderGraph* graph){
	VectorOrderNode finishNodes;
	initDefVectorOrderNode(& finishNodes);
	HashTableNode* nodeToInfo = allocHashTableNode(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	initializeNodeInfoSCC(graph, nodeToInfo);
	// TODO
	deleteHashTableNode(nodeToInfo);
}

void orderAnalysis(CSolver* solver){
	OrderEncoder* oEncoder = buildOrderGraphs(solver);
	uint size = getSizeVectorOrderGraph(&oEncoder->graphs);
	for(uint i=0; i<size; i++){
		OrderGraph* graph = getVectorOrderGraph(&oEncoder->graphs, i);
		computeStronglyConnectedComponentGraph(graph);
	}
}

