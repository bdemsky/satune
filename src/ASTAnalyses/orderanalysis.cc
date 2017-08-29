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

void DFS(OrderGraph *graph, Vector<OrderNode *> *finishNodes) {
	HSIteratorOrderNode *iterator = graph->getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = iterator->next();
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, false, 0);
			node->status = FINISHED;
			finishNodes->push(node);
		}
	}
	delete iterator;
}

void DFSReverse(OrderGraph *graph, Vector<OrderNode *> *finishNodes) {
	uint size = finishNodes->getSize();
	uint sccNum = 1;
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes->get(i);
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, NULL, true, false, sccNum);
			node->sccNum = sccNum;
			node->status = FINISHED;
			sccNum++;
		}
	}
}

void DFSNodeVisit(OrderNode *node, Vector<OrderNode *> *finishNodes, bool isReverse, bool mustvisit, uint sccNum) {
	HSIteratorOrderEdge *iterator = isReverse ? node->inEdges.iterator() : node->outEdges.iterator();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (mustvisit) {
			if (!edge->mustPos)
				continue;
		} else
		if (!edge->polPos && !edge->pseudoPos)	//Ignore edges that do not have positive polarity
			continue;

		OrderNode *child = isReverse ? edge->source : edge->sink;

		if (child->status == NOTVISITED) {
			child->status = VISITED;
			DFSNodeVisit(child, finishNodes, isReverse, mustvisit, sccNum);
			child->status = FINISHED;
			if (finishNodes != NULL)
				finishNodes->push(child);
			if (isReverse)
				child->sccNum = sccNum;
		}
	}
	delete iterator;
}

void resetNodeInfoStatusSCC(OrderGraph *graph) {
	HSIteratorOrderNode *iterator = graph->getNodes();
	while (iterator->hasNext()) {
		iterator->next()->status = NOTVISITED;
	}
	delete iterator;
}

void computeStronglyConnectedComponentGraph(OrderGraph *graph) {
	Vector<OrderNode *> finishNodes;
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	DFSReverse(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
}

bool isMustBeTrueNode(OrderNode *node) {
	HSIteratorOrderEdge *iterator = node->inEdges.iterator();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (!edge->mustPos) {
			delete iterator;
			return false;
		}
	}
	delete iterator;
	iterator = node->outEdges.iterator();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (!edge->mustPos) {
			delete iterator;
			return false;
		}
	}
	delete iterator;
	return true;
}

void bypassMustBeTrueNode(CSolver *This, OrderGraph *graph, OrderNode *node) {
	HSIteratorOrderEdge *iterin = node->inEdges.iterator();
	while (iterin->hasNext()) {
		OrderEdge *inEdge = iterin->next();
		OrderNode *srcNode = inEdge->source;
		srcNode->outEdges.remove(inEdge);
		HSIteratorOrderEdge *iterout = node->outEdges.iterator();
		while (iterout->hasNext()) {
			OrderEdge *outEdge = iterout->next();
			OrderNode *sinkNode = outEdge->sink;
			sinkNode->inEdges.remove(outEdge);
			//Adding new edge to new sink and src nodes ...
			OrderEdge *newEdge = graph->getOrderEdgeFromOrderGraph(srcNode, sinkNode);
			newEdge->mustPos = true;
			newEdge->polPos = true;
			if (newEdge->mustNeg)
				This->setUnSAT();
			srcNode->outEdges.add(newEdge);
			sinkNode->inEdges.add(newEdge);
		}
		delete iterout;
	}
	delete iterin;
}

void removeMustBeTrueNodes(CSolver *This, OrderGraph *graph) {
	HSIteratorOrderNode *iterator = graph->getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = iterator->next();
		if (isMustBeTrueNode(node)) {
			bypassMustBeTrueNode(This, graph, node);
		}
	}
	delete iterator;
}

/** This function computes a source set for every nodes, the set of
    nodes that can reach that node via pospolarity edges.  It then
    looks for negative polarity edges from nodes in the the source set
    to determine whether we need to generate pseudoPos edges. */

void completePartialOrderGraph(OrderGraph *graph) {
	Vector<OrderNode *> finishNodes;
	DFS(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);
	HashTableNodeToNodeSet *table = new HashTableNodeToNodeSet(128, 0.25);

	Vector<OrderNode *> sccNodes;

	uint size = finishNodes.getSize();
	uint sccNum = 1;
	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes.get(i);
		HashSetOrderNode *sources = new HashSetOrderNode(4, 0.25);
		table->put(node, sources);

		if (node->status == NOTVISITED) {
			//Need to do reverse traversal here...
			node->status = VISITED;
			DFSNodeVisit(node, &sccNodes, true, false, sccNum);
			node->status = FINISHED;
			node->sccNum = sccNum;
			sccNum++;
			sccNodes.push(node);

			//Compute in set for entire SCC
			uint rSize = sccNodes.getSize();
			for (uint j = 0; j < rSize; j++) {
				OrderNode *rnode = sccNodes.get(j);
				//Compute source sets
				HSIteratorOrderEdge *iterator = rnode->inEdges.iterator();
				while (iterator->hasNext()) {
					OrderEdge *edge = iterator->next();
					OrderNode *parent = edge->source;
					if (edge->polPos) {
						sources->add(parent);
						HashSetOrderNode *parent_srcs = (HashSetOrderNode *)table->get(parent);
						sources->addAll(parent_srcs);
					}
				}
				delete iterator;
			}
			for (uint j = 0; j < rSize; j++) {
				//Copy in set of entire SCC
				OrderNode *rnode = sccNodes.get(j);
				HashSetOrderNode *set = (j == 0) ? sources : sources->copy();
				table->put(rnode, set);

				//Use source sets to compute pseudoPos edges
				HSIteratorOrderEdge *iterator = node->inEdges.iterator();
				while (iterator->hasNext()) {
					OrderEdge *edge = iterator->next();
					OrderNode *parent = edge->source;
					ASSERT(parent != rnode);
					if (edge->polNeg && parent->sccNum != rnode->sccNum &&
							sources->contains(parent)) {
						OrderEdge *newedge = graph->getOrderEdgeFromOrderGraph(rnode, parent);
						newedge->pseudoPos = true;
					}
				}
				delete iterator;
			}

			sccNodes.clear();
		}
	}

	table->resetanddelete();
	delete table;
	resetNodeInfoStatusSCC(graph);
}

void DFSMust(OrderGraph *graph, Vector<OrderNode *> *finishNodes) {
	HSIteratorOrderNode *iterator = graph->getNodes();
	while (iterator->hasNext()) {
		OrderNode *node = iterator->next();
		if (node->status == NOTVISITED) {
			node->status = VISITED;
			DFSNodeVisit(node, finishNodes, false, true, 0);
			node->status = FINISHED;
			finishNodes->push(node);
		}
	}
	delete iterator;
}

void DFSClearContradictions(CSolver *solver, OrderGraph *graph, Vector<OrderNode *> *finishNodes, bool computeTransitiveClosure) {
	uint size = finishNodes->getSize();
	HashTableNodeToNodeSet *table = new HashTableNodeToNodeSet(128, 0.25);

	for (int i = size - 1; i >= 0; i--) {
		OrderNode *node = finishNodes->get(i);
		HashSetOrderNode *sources = new HashSetOrderNode(4, 0.25);
		table->put(node, sources);

		{
			//Compute source sets
			HSIteratorOrderEdge *iterator = node->inEdges.iterator();
			while (iterator->hasNext()) {
				OrderEdge *edge = iterator->next();
				OrderNode *parent = edge->source;
				if (edge->mustPos) {
					sources->add(parent);
					HashSetOrderNode *parent_srcs = (HashSetOrderNode *) table->get(parent);
					sources->addAll(parent_srcs);
				}
			}
			delete iterator;
		}
		if (computeTransitiveClosure) {
			//Compute full transitive closure for nodes
			HSIteratorOrderNode *srciterator = sources->iterator();
			while (srciterator->hasNext()) {
				OrderNode *srcnode = srciterator->next();
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
			HSIteratorOrderEdge *iterator = node->inEdges.iterator();
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
			HSIteratorOrderEdge *iterator = node->outEdges.iterator();
			while (iterator->hasNext()) {
				OrderEdge *edge = iterator->next();
				OrderNode *child = edge->sink;
				if (!edge->mustNeg && sources->contains(child)) {
					edge->mustNeg = true;
					edge->polNeg = true;
					if (edge->mustPos)
						solver->setUnSAT();
				}
			}
			delete iterator;
		}
	}

	table->resetanddelete();
	delete table;
}

/* This function finds edges that would form a cycle with must edges
   and forces them to be mustNeg.  It also decides whether an edge
   must be true because of transitivity from other must be true
   edges. */

void reachMustAnalysis(CSolver *solver, OrderGraph *graph, bool computeTransitiveClosure) {
	Vector<OrderNode *> finishNodes;
	//Topologically sort the mustPos edge graph
	DFSMust(graph, &finishNodes);
	resetNodeInfoStatusSCC(graph);

	//Find any backwards edges that complete cycles and force them to be mustNeg
	DFSClearContradictions(solver, graph, &finishNodes, computeTransitiveClosure);
}

/* This function finds edges that must be positive and forces the
   inverse edge to be negative (and clears its positive polarity if it
   had one). */

void localMustAnalysisTotal(CSolver *solver, OrderGraph *graph) {
	HSIteratorOrderEdge *iterator = graph->getEdges();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (edge->mustPos) {
			OrderEdge *invEdge = graph->getInverseOrderEdge(edge);
			if (invEdge != NULL) {
				if (!invEdge->mustPos) {
					invEdge->polPos = false;
				} else {
					solver->setUnSAT();
				}
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
	HSIteratorOrderEdge *iterator = graph->getEdges();
	while (iterator->hasNext()) {
		OrderEdge *edge = iterator->next();
		if (edge->mustPos) {
			if (!edge->mustNeg) {
				edge->polNeg = false;
			} else
				solver->setUnSAT();

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
