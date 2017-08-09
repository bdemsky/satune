
#include "ordergraphbuilder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "ordergraph.h"


void buildOrderGraph(CSolver* This){
	uint size = getSizeVectorBoolean(This->constraints);
	OrderGraph* orderGraph = allocOrderGraph();
	for(uint i=0; i<size; i++){
		Boolean* constraint = getVectorBoolean(This->constraints, i);
		if(GETBOOLEANTYPE(constraint) == ORDERCONST){
			addOrderConstraintToOrderGraph(orderGraph, constraint);
		}
	}
	//TODO: We should add the orderGraph to our encoder
	deleteOrderGraph(orderGraph);
}

