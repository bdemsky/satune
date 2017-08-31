#include "transformer.h"
#include "common.h"
#include "order.h"
#include "boolean.h"
#include "ordergraph.h"
#include "ordernode.h"
#include "rewriter.h"
#include "orderedge.h"
#include "mutableset.h"
#include "ops.h"
#include "csolver.h"
#include "orderanalysis.h"
#include "tunable.h"
#include "transform.h"
#include "element.h"
#include "integerencoding.h"
#include "decomposeordertransform.h"

Transformer::Transformer(CSolver *_solver):
	integerEncoding(new IntegerEncodingTransform(_solver)),
	solver(_solver)
{
}

Transformer::~Transformer(){
	delete integerEncoding;
}

void Transformer::orderAnalysis() {
	Vector<Order *> *orders = solver->getOrders();
	uint size = orders->getSize();
	for (uint i = 0; i < size; i++) {
		Order *order = orders->get(i);
		DecomposeOrderTransform* decompose = new DecomposeOrderTransform(solver, order);
		if (!decompose->canExecuteTransform()){
			delete decompose;
			continue;
		}

		OrderGraph *graph = buildOrderGraph(order);
		if (order->type == PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			completePartialOrderGraph(graph);
		}


		bool mustReachGlobal = GETVARTUNABLE(solver->getTuner(), order->type, MUSTREACHGLOBAL, &onoff);

		if (mustReachGlobal)
			reachMustAnalysis(solver, graph, false);

		bool mustReachLocal = GETVARTUNABLE(solver->getTuner(), order->type, MUSTREACHLOCAL, &onoff);

		if (mustReachLocal) {
			//solver pair of analysis is also optional
			if (order->type == PARTIAL) {
				localMustAnalysisPartial(solver, graph);
			} else {
				localMustAnalysisTotal(solver, graph);
			}
		}

		bool mustReachPrune = GETVARTUNABLE(solver->getTuner(), order->type, MUSTREACHPRUNE, &onoff);

		if (mustReachPrune)
			removeMustBeTrueNodes(solver, graph);

		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);
		decompose->setOrderGraph(graph);
		decompose->doTransform();
		delete decompose;
		delete graph;

		integerEncoding->setCurrentOrder(order);
		if(!integerEncoding->canExecuteTransform()){
			continue;
		}
		integerEncoding->doTransform();
 	}
}


