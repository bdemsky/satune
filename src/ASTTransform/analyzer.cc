#include "analyzer.h"
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
#include "orderencoder.h"
#include "tunable.h"
#include "transform.h"
#include "element.h"
#include "integerencoding.h"
#include "decomposeordertransform.h"

void orderAnalysis(CSolver *This) {
	Vector<Order *> *orders = This->getOrders();
	uint size = orders->getSize();
	for (uint i = 0; i < size; i++) {
		Order *order = orders->get(i);
		DecomposeOrderTransform* decompose = new DecomposeOrderTransform(This, order, DECOMPOSEORDER, &onoff);
		if (!decompose->canExecuteTransform()){
			continue;
			delete decompose;
		}

		OrderGraph *graph = buildOrderGraph(order);
		if (order->type == PARTIAL) {
			//Required to do SCC analysis for partial order graphs.  It
			//makes sure we don't incorrectly optimize graphs with negative
			//polarity edges
			completePartialOrderGraph(graph);
		}


		bool mustReachGlobal = GETVARTUNABLE(This->getTuner(), order->type, MUSTREACHGLOBAL, &onoff);

		if (mustReachGlobal)
			reachMustAnalysis(This, graph, false);

		bool mustReachLocal = GETVARTUNABLE(This->getTuner(), order->type, MUSTREACHLOCAL, &onoff);

		if (mustReachLocal) {
			//This pair of analysis is also optional
			if (order->type == PARTIAL) {
				localMustAnalysisPartial(This, graph);
			} else {
				localMustAnalysisTotal(This, graph);
			}
		}

		bool mustReachPrune = GETVARTUNABLE(This->getTuner(), order->type, MUSTREACHPRUNE, &onoff);

		if (mustReachPrune)
			removeMustBeTrueNodes(This, graph);

		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);
		decompose->setOrderGraph(graph);
		decompose->doTransform();
		delete decompose;
		delete graph;

		
		IntegerEncodingTransform* integerEncoding = new IntegerEncodingTransform(This, order, ORDERINTEGERENCODING, &offon);
		if(!integerEncoding->canExecuteTransform()){
			continue;
			delete integerEncoding;
		}
		integerEncoding->doTransform();
		delete integerEncoding;
	}
}


