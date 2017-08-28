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

Edge SATEncoder::encodeOrderSATEncoder(BooleanOrder *constraint) {
	switch ( constraint->order->type) {
	case PARTIAL:
		return encodePartialOrderSATEncoder(constraint);
	case TOTAL:
		return encodeTotalOrderSATEncoder(constraint);
	default:
		ASSERT(0);
	}
	return E_BOGUS;
}

Edge inferOrderConstraintFromGraph(Order *order, uint64_t _first, uint64_t _second) {
	if (order->graph != NULL) {
		OrderGraph *graph = order->graph;
		OrderNode *first = graph->lookupOrderNodeFromOrderGraph(_first);
		OrderNode *second = graph->lookupOrderNodeFromOrderGraph(_second);
		if ((first != NULL) && (second != NULL)) {
			OrderEdge *edge = graph->lookupOrderEdgeFromOrderGraph(first, second);
			if (edge != NULL) {
				if (edge->mustPos)
					return E_True;
				else if (edge->mustNeg)
					return E_False;
			}
			OrderEdge *invedge = graph->lookupOrderEdgeFromOrderGraph(second, first);
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

Edge SATEncoder::getPairConstraint(Order *order, OrderPair *pair) {
	Edge gvalue = inferOrderConstraintFromGraph(order, pair->first, pair->second);
	if (!edgeIsNull(gvalue))
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
		constraint = getNewVarSATEncoder();
		OrderPair *paircopy = new OrderPair(pair->first, pair->second, constraint);
		table->put(paircopy, paircopy);
	} else
		constraint = table->get(pair)->constraint;

	return negate ? constraintNegate(constraint) : constraint;
}

Edge SATEncoder::encodeTotalOrderSATEncoder(BooleanOrder *boolOrder) {
	ASSERT(boolOrder->order->type == TOTAL);
	if (boolOrder->order->orderPairTable == NULL) {
		boolOrder->order->initializeOrderHashTable();
		bool doOptOrderStructure = GETVARTUNABLE(solver->getTuner(), boolOrder->order->type, OPTIMIZEORDERSTRUCTURE, &onoff);
		if (doOptOrderStructure) {
			boolOrder->order->graph = buildMustOrderGraph(boolOrder->order);
			reachMustAnalysis(solver, boolOrder->order->graph, true);
		}
		createAllTotalOrderConstraintsSATEncoder(boolOrder->order);
	}
	OrderPair pair(boolOrder->first, boolOrder->second, E_NULL);
	Edge constraint = getPairConstraint(boolOrder->order, &pair);
	return constraint;
}


void SATEncoder::createAllTotalOrderConstraintsSATEncoder(Order *order) {
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
			Edge constIJ = getPairConstraint(order, &pairIJ);
			for (uint k = j + 1; k < size; k++) {
				uint64_t valueK = mems->get(k);
				OrderPair pairJK(valueJ, valueK, E_NULL);
				OrderPair pairIK(valueI, valueK, E_NULL);
				Edge constIK = getPairConstraint(order, &pairIK);
				Edge constJK = getPairConstraint(order, &pairJK);
				addConstraintCNF(cnf, generateTransOrderConstraintSATEncoder(constIJ, constJK, constIK));
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

Edge SATEncoder::generateTransOrderConstraintSATEncoder(Edge constIJ,Edge constJK,Edge constIK) {
	Edge carray[] = {constIJ, constJK, constraintNegate(constIK)};
	Edge loop1 = constraintOR(cnf, 3, carray);
	Edge carray2[] = {constraintNegate(constIJ), constraintNegate(constJK), constIK};
	Edge loop2 = constraintOR(cnf, 3, carray2 );
	return constraintAND2(cnf, loop1, loop2);
}

Edge SATEncoder::encodePartialOrderSATEncoder(BooleanOrder *constraint) {
	ASSERT(constraint->order->type == PARTIAL);
	return E_BOGUS;
}
