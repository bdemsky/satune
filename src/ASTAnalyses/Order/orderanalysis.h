#ifndef ORDERANALYSIS_H
#define ORDERANALYSIS_H
#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

void DFSClearContradictions(CSolver *solver, OrderGraph *graph, Vector<OrderNode *> *finishNodes, bool computeTransitiveClosure);
void reachMustAnalysis(CSolver *solver, OrderGraph *graph, bool computeTransitiveClosure);
void localMustAnalysisTotal(CSolver *solver, OrderGraph *graph);
void localMustAnalysisPartial(CSolver *solver, OrderGraph *graph);

#endif

