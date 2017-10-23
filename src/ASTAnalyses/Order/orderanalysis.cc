#include <stdint.h>

#include "orderanalysis.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"
#include "order.h"
#include "ordernode.h"
#include "rewriter.h"
#include "mutableset.h"
#include "tunable.h"

void DFSClearContradictions(CSolver *solver, OrderGraph *graph, Vector<OrderNode *> *finishNodes, bool computeTransitiveClosure) {
	uint size = finishNodes->getSize();
	HashtableNodeToNodeSet *table = new HashtableNodeToNodeSet(128, 0.25);

	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes->get(i);
		HashsetOrderNode *sources = new HashsetOrderNode(4, 0.25);
		table->put(node, sources);

		{
			//Compute source sets
			SetIteratorOrderEdge *iterator = node->inEdges.iterator();
			while (iterator->hasNext()) {
				OrderEdge *edge = iterator->next();
				OrderNode *parent = edge->source;
				if (edge->mustPos) {
					sources->add(parent);
					HashsetOrderNode *parent_srcs = (HashsetOrderNode *) table->get(parent);
					sources->addAll(parent_srcs);
				}
			}
			delete iterator;
		}
		if (computeTransitiveClosure) {
			//Compute full transitive closure for nodes
			SetIteratorOrderNode *srciterator = sources->iterator();
			while (srciterator->hasNext()) {
				OrderNode *srcnode = srciterator->next();
				if (srcnode->removed)
					continue;
				OrderEdge *newedge = graph->getOrderEdgeFromOrderGraph(srcnode, node);
				newedge->mustPos = true;
				newedge->polPos = true;
				if (newedge->mustNeg)
					solver->setUnSAT();
				srcnode->outEdges.add(newedge);
				node->inEdges.add(newedge);
			}
			delete srciterator;
		}
		{
			//Use source sets to compute mustPos edges
			SetIteratorOrderEdge *iterator = node->inEdges.iterator();
			while (iterator->hasNext()) {
				OrderEdge *edge = iterator->next();
				OrderNode *parent = edge->source;
				if (!edge->mustPos && sources->contains(parent)) {
					edge->mustPos = true;
					edge->polPos = true;
					if (edge->mustNeg)
						solver->setUnSAT();
				}
			}
			delete iterator;
		}
		{
			//Use source sets to compute mustNeg for edges that would introduce cycle if true
			SetIteratorOrderEdge *iterator = node->outEdges.iterator();
			while (iterator->hasNext()) {
				OrderEdge *edge = iterator->next();
				OrderNode *child = edge->sink;
				if (!edge->mustNeg && sources->contains(child)) {
					edge->mustNeg = true;
					edge->polNeg = true;
					if (edge->mustPos) {
						solver->setUnSAT();
					}
				}
			}
			delete iterator;
		}
	}

	table->resetAndDeleteVals();
	delete table;
}

/* This function finds edges that would form a cycle with must edges
   and forces them to be mustNeg.  It also decides whether an edge
   must be true because of transitivity from other must be true
   edges. */

void reachMustAnalysis(CSolver *solver, OrderGraph *graph, bool computeTransitiveClosure) {
	Vector<OrderNode *> finishNodes;
	//Topologically sort the mustPos edge graph
	graph->DFSMust(&finishNodes);
	graph->resetNodeInfoStatusSCC();

	//Find any backwards edges that complete cycles and force them to be mustNeg
	DFSClearContradictions(solver, graph, &finishNodes, computeTransitiveClosure);
}

/* This function finds edges that must be positive and forces the
   inverse edge to be negative (and clears its positive polarity if it
   had one). */

void localMustAnalysisTotal(CSolver *solver, OrderGraph *graph) {
	SetIteratorOrderEdge *iterator = graph->getEdges();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (edge->mustPos) {
			OrderEdge *invEdge = graph->getInverseOrderEdge(edge);
			if (invEdge != NULL) {
				if (!invEdge->mustPos) {
					invEdge->polPos = false;
				} else
					solver->setUnSAT();
				invEdge->mustNeg = true;
				invEdge->polNeg = true;
			}
		}
	}
	delete iterator;
}

/** This finds edges that must be positive and forces the inverse edge
    to be negative.  It also clears the negative flag of this edge.
    It also finds edges that must be negative and clears the positive
    polarity. */

void localMustAnalysisPartial(CSolver *solver, OrderGraph *graph) {
	SetIteratorOrderEdge *iterator = graph->getEdges();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (edge->mustPos) {
			if (!edge->mustNeg) {
				edge->polNeg = false;
			} else {
				solver->setUnSAT();
			}
			OrderEdge *invEdge = graph->getInverseOrderEdge(edge);
			if (invEdge != NULL) {
				if (!invEdge->mustPos)
					invEdge->polPos = false;
				else
					solver->setUnSAT();
				invEdge->mustNeg = true;
				invEdge->polNeg = true;
			}
		}
		if (edge->mustNeg && !edge->mustPos) {
			edge->polPos = false;
		}
	}
	delete iterator;
}
