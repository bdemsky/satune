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

struct NodeInfo {
	NodeStatus status;
};

NodeInfo* allocNodeInfo();
void deleteNodeInfo(NodeInfo* info);

OrderGraph* buildOrderGraph(Order *order);
void computeStronglyConnectedComponentGraph(OrderGraph* graph);
void orderAnalysis(CSolver* solver);
void initializeNodeInfoSCC(OrderGraph* graph, HashTableNodeInfo* nodeToInfo);
void DFSNodeVisit(OrderNode* node, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo, bool isReverse);
void DFS(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo);
void DFSReverse(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo);
void completePartialOrderGraph(OrderGraph* graph);
void resetNodeInfoStatusSCC(OrderGraph* graph, HashTableNodeInfo* nodeToInfo);
void removeMustBeTrueNodes(OrderGraph* graph);
void DFSPseudoNodeVisit(OrderGraph *graph, OrderNode* node, HashTableNodeInfo* nodeToInfo);
void completePartialOrderGraph(OrderGraph* graph);
void DFSMust(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo);
void DFSMustNodeVisit(OrderNode* node, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo, bool clearBackEdges);
void DFSClearContradictions(OrderGraph* graph, VectorOrderNode* finishNodes, HashTableNodeInfo* nodeToInfo);
void reachMustAnalysis(OrderGraph *graph);
void localMustAnalysisTotal(OrderGraph *graph);
void localMustAnalysisPartial(OrderGraph *graph);
void orderAnalysis(CSolver* This);

#endif /* ORDERGRAPHBUILDER_H */

