/* 
 * File:   orderencoder.h
 * Author: hamed
 *
 * Created on August 8, 2017, 6:36 PM
 */

#ifndef ORDERGRAPHBUILDER_H
#define ORDERGRAPHBUILDER_H
#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

enum NodeStatus {NOTVISITED, VISITED, FINISHED};
typedef enum NodeStatus NodeStatus;

struct NodeInfo{
	NodeStatus status;
	uint finishTime;
};

struct OrderEncoder{
	VectorOrderGraph graphs;
};

NodeInfo* allocNodeInfo();
void deleteNodeInfo(NodeInfo* info);
OrderEncoder* allocOrderEncoder();
void deleteOrderEncoder(OrderEncoder* This);

OrderEncoder* buildOrderGraphs(CSolver* This);
void computeStronglyConnectedComponentGraph(OrderGraph* graph);
void orderAnalysis(CSolver* solver);
void initializeNodeInfoSCC(OrderGraph* graph, HashTableNodeInfo* nodeToInfo);
void DFSNodeVisit(OrderNode* node, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo, uint* timer, bool isReverse);
void DFS(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo);
void DFSReverse(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo);

#endif /* ORDERGRAPHBUILDER_H */

