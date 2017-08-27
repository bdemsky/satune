#include "satencoder.h"
#include "structs.h"
#include "common.h"
#include "order.h"
#include "csolver.h"
#include "orderpair.h"
#include "set.h"
#include "tunable.h"
#include "orderencoder.h"
#include "ordergraph.h"
#include "orderedge.h"
#include "element.h"
#include "predicate.h"
#include "orderelement.h"

Edge encodeOrderSATEncoder(SATEncoder *This, BooleanOrder *constraint) {
	switch ( constraint->order->type) {
		case PARTIAL:
			return encodePartialOrderSATEncoder(This, constraint);
		case TOTAL:
			return encodeTotalOrderSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge inferOrderConstraintFromGraph(Order* order, uint64_t _first, uint64_t _second){
	if (order->graph != NULL) {
		OrderGraph *graph=order->graph;
		OrderNode *first=graph->lookupOrderNodeFromOrderGraph(_first);
		OrderNode *second=graph->lookupOrderNodeFromOrderGraph(_second);
		if ((first != NULL) && (second != NULL)) {
			OrderEdge *edge=graph->lookupOrderEdgeFromOrderGraph(first, second);
			if (edge != NULL) {
				if (edge->mustPos)
					return E_True;
				else if (edge->mustNeg)
					return E_False;
			}
			OrderEdge *invedge=graph->lookupOrderEdgeFromOrderGraph(second, first);
			if (invedge != NULL) {
				if (invedge->mustPos)
					return E_False;
				else if (invedge->mustNeg)
					return E_True;
			}
		}
	}
	return E_NULL;
}

Edge getPairConstraint(SATEncoder *This, Order *order, OrderPair *pair) {
	Edge gvalue = inferOrderConstraintFromGraph(order, pair->first, pair->second);
	if(!edgeIsNull(gvalue))
		return gvalue;
	
	HashTableOrderPair *table = order->orderPairTable;
	bool negate = false;
	OrderPair flipped;
	if (pair->first < pair->second) {
		negate = true;
		flipped.first = pair->second;
		flipped.second = pair->first;
		pair = &flipped;
	}
	Edge constraint;
	if (!(table->contains(pair))) {
		constraint = getNewVarSATEncoder(This);
		OrderPair *paircopy = new OrderPair(pair->first, pair->second, constraint);
		table->put(paircopy, paircopy);
	} else
		constraint = table->get(pair)->constraint;
	
	return negate ? constraintNegate(constraint) : constraint;
}

Edge encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder *boolOrder) {
	ASSERT(boolOrder->order->type == TOTAL);
	if (boolOrder->order->orderPairTable == NULL) {
		boolOrder->order->initializeOrderHashTable();
		bool doOptOrderStructure=GETVARTUNABLE(This->solver->getTuner(), boolOrder->order->type, OPTIMIZEORDERSTRUCTURE, &onoff);
		if (doOptOrderStructure) {
			boolOrder->order->graph = buildMustOrderGraph(boolOrder->order);
			reachMustAnalysis(This->solver, boolOrder->order->graph, true);
		}
		createAllTotalOrderConstraintsSATEncoder(This, boolOrder->order);
	}
	OrderPair pair(boolOrder->first, boolOrder->second, E_NULL);
	Edge constraint = getPairConstraint(This, boolOrder->order, &pair);
	return constraint;
}


void createAllTotalOrderConstraintsSATEncoder(SATEncoder *This, Order *order) {
#ifdef TRACE_DEBUG
	model_print("in total order ...\n");
#endif
	ASSERT(order->type == TOTAL);
	Vector<uint64_t> *mems = order->set->members;
	uint size = mems->getSize();
	for (uint i = 0; i < size; i++) {
		uint64_t valueI = mems->get(i);
		for (uint j = i + 1; j < size; j++) {
			uint64_t valueJ = mems->get(j);
			OrderPair pairIJ(valueI, valueJ, E_NULL);
			Edge constIJ = getPairConstraint(This, order, &pairIJ);
			for (uint k = j + 1; k < size; k++) {
				uint64_t valueK = mems->get(k);
				OrderPair pairJK(valueJ, valueK, E_NULL);
				OrderPair pairIK(valueI, valueK, E_NULL);
				Edge constIK = getPairConstraint(This, order, &pairIK);
				Edge constJK = getPairConstraint(This, order, &pairJK);
				addConstraintCNF(This->cnf, generateTransOrderConstraintSATEncoder(This, constIJ, constJK, constIK));
			}
		}
	}
}

Edge getOrderConstraint(HashTableOrderPair *table, OrderPair *pair) {
	ASSERT(pair->first != pair->second);
	bool negate = false;
	OrderPair flipped;
	if (pair->first < pair->second) {
		negate = true;
		flipped.first = pair->second;
		flipped.second = pair->first;
		pair = &flipped;
	}
	if (!table->contains(pair)) {
		return E_NULL;
	}
	Edge constraint = table->get(pair)->constraint;
	ASSERT(!edgeIsNull(constraint));
	return negate ? constraintNegate(constraint) : constraint;
}

Edge generateTransOrderConstraintSATEncoder(SATEncoder *This, Edge constIJ,Edge constJK,Edge constIK) {
	Edge carray[] = {constIJ, constJK, constraintNegate(constIK)};
	Edge loop1 = constraintOR(This->cnf, 3, carray);
	Edge carray2[] = {constraintNegate(constIJ), constraintNegate(constJK), constIK};
	Edge loop2 = constraintOR(This->cnf, 3, carray2 );
	return constraintAND2(This->cnf, loop1, loop2);
}

Edge encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder *constraint) {
	ASSERT(constraint->order->type == PARTIAL);
	return E_BOGUS;
}
