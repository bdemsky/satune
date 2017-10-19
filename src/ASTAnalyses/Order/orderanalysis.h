#ifndef ORDERANALYSIS_H
#define ORDERANALYSIS_H
#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

void computeStronglyConnectedComponentGraph(OrderGraph *graph);
void initializeNodeInfoSCC(OrderGraph *graph);
void DFSNodeVisit(OrderNode *node, Vector<OrderNode *> *finishNodes, bool isReverse, bool mustvisit, uint sccNum);
void DFS(OrderGraph *graph, Vector<OrderNode *> *finishNodes);
void DFSReverse(OrderGraph *graph, Vector<OrderNode *> *finishNodes);
void completePartialOrderGraph(OrderGraph *graph);
void resetNodeInfoStatusSCC(OrderGraph *graph);
void DFSMust(OrderGraph *graph, Vector<OrderNode *> *finishNodes);
void DFSClearContradictions(CSolver *solver, OrderGraph *graph, Vector<OrderNode *> *finishNodes, bool computeTransitiveClosure);
void reachMustAnalysis(CSolver *solver, OrderGraph *graph, bool computeTransitiveClosure);
void localMustAnalysisTotal(CSolver *solver, OrderGraph *graph);
void localMustAnalysisPartial(CSolver *solver, OrderGraph *graph);

#endif

