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

void computeStronglyConnectedComponentGraph(OrderGraph *graph);
void orderAnalysis(CSolver *solver);
void initializeNodeInfoSCC(OrderGraph *graph);
void DFSNodeVisit(OrderNode *node, Vector<OrderNode *> *finishNodes, bool isReverse, bool mustvisit, uint sccNum);
void DFS(OrderGraph *graph, Vector<OrderNode *> *finishNodes);
void DFSReverse(OrderGraph *graph, Vector<OrderNode *> *finishNodes);
void completePartialOrderGraph(OrderGraph *graph);
void resetNodeInfoStatusSCC(OrderGraph *graph);
bool isMustBeTrueNode(OrderNode* node);
void bypassMustBeTrueNode(CSolver *This, OrderGraph* graph, OrderNode* node);
void removeMustBeTrueNodes(CSolver *This, OrderGraph *graph);
void completePartialOrderGraph(OrderGraph *graph);
void DFSMust(OrderGraph *graph, Vector<OrderNode *> *finishNodes);
void DFSClearContradictions(CSolver *solver, OrderGraph *graph, Vector<OrderNode *> *finishNodes, bool computeTransitiveClosure);
void reachMustAnalysis(CSolver *solver, OrderGraph *graph, bool computeTransitiveClosure);
void localMustAnalysisTotal(CSolver *solver, OrderGraph *graph);
void localMustAnalysisPartial(CSolver *solver, OrderGraph *graph);

#endif/* ORDERGRAPHBUILDER_H */

