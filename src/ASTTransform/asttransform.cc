#include "asttransform.h"
#include "order.h"
#include "tunable.h"
#include "csolver.h"
#include "ordergraph.h"
#include "orderencoder.h"
#include "orderdecompose.h"
#include "integerencoding.h"

void ASTTransform(CSolver *This){
	uint size = This->allOrders.getSize();
	for (uint i = 0; i < size; i++) {
		Order *order = This->allOrders.get(i);
		bool doDecompose=GETVARTUNABLE(This->tuner, order->type, DECOMPOSEORDER, &onoff);
		if (!doDecompose)
			continue;
		
		OrderGraph *graph;
		if(order->graph == NULL){
			graph= buildOrderGraph(order);
			if (order->type == PARTIAL) {
				//Required to do SCC analysis for partial order graphs.  It
				//makes sure we don't incorrectly optimize graphs with negative
				//polarity edges
				completePartialOrderGraph(graph);
			}
		}else{
			graph = order->graph;
		}		
		//This is needed for splitorder
		computeStronglyConnectedComponentGraph(graph);
		decomposeOrder(This, order, graph);
		
		bool doIntegerEncoding = GETVARTUNABLE(This->tuner, order->order.type, ORDERINTEGERENCODING, &onoff );
		if(!doIntegerEncoding)
			continue;
		uint size = order->constraints.getSize();
		for(uint i=0; i<size; i++){
			orderIntegerEncodingSATEncoder(This->satEncoder, order->constraints.get(i));
		}
	}
	
	
}

