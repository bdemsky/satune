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

OrderGraph *buildOrderGraph(Order *order);
void computeStronglyConnectedComponentGraph(OrderGraph *graph);
void orderAnalysis(CSolver *solver);
void initializeNodeInfoSCC(OrderGraph *graph);
void DFSNodeVisit(OrderNode *node, VectorOrderNode *finishNodes, bool isReverse, uint sccNum);
void DFS(OrderGraph *graph, VectorOrderNode *finishNodes);
void DFSReverse(OrderGraph *graph, VectorOrderNode *finishNodes);
void completePartialOrderGraph(OrderGraph *graph);
void resetNodeInfoStatusSCC(OrderGraph *graph);
void removeMustBeTrueNodes(OrderGraph *graph);
void DFSPseudoNodeVisit(OrderGraph *graph, OrderNode *node);
void completePartialOrderGraph(OrderGraph *graph);
void DFSMust(OrderGraph *graph, VectorOrderNode *finishNodes);
void DFSMustNodeVisit(OrderNode *node, VectorOrderNode *finishNodes, bool clearBackEdges);
void DFSClearContradictions(OrderGraph *graph, VectorOrderNode *finishNodes);
void reachMustAnalysis(OrderGraph *graph);
void localMustAnalysisTotal(OrderGraph *graph);
void localMustAnalysisPartial(OrderGraph *graph);
void orderAnalysis(CSolver *This);
void decomposeOrder(Order *order, OrderGraph *graph);

#endif/* ORDERGRAPHBUILDER_H */

