#include "satencoder.h"
#include "structs.h"
#include "common.h"
#include "order.h"
#include "csolver.h"
#include "orderpair.h"
#include "set.h"
#include "tunable.h"
#include "orderanalysis.h"
#include "ordergraph.h"
#include "orderedge.h"
#include "element.h"
#include "predicate.h"
#include "orderelement.h"
#include "orderpairresolver.h"

Edge SATEncoder::encodeOrderSATEncoder(BooleanOrder *constraint) {
	switch ( constraint->order->type) {
	case SATC_PARTIAL:
		return encodePartialOrderSATEncoder(constraint);
	case SATC_TOTAL:
		return encodeTotalOrderSATEncoder(constraint);
	default:
		ASSERT(0);
	}
	return E_BOGUS;
}

Edge SATEncoder::inferOrderConstraintFromGraph(Order *order, uint64_t _first, uint64_t _second) {
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

	HashtableOrderPair *table = order->getOrderPairTable();
	bool negate = false;
	OrderPair flipped;
	if (pair->first < pair->second) {
		negate = true;
		flipped.first = pair->second;
		flipped.second = pair->first;
		pair = &flipped;
	}
	OrderPair* tmp;
	if (!(table->contains(pair))) {
		tmp = new OrderPair(pair->first, pair->second, getNewVarSATEncoder());
		table->put(tmp, tmp);
	} else {
		tmp = table->get(pair);
	}
	return negate ? tmp->getNegatedConstraint() : tmp->getConstraint();
}

Edge SATEncoder::encodeTotalOrderSATEncoder(BooleanOrder *boolOrder) {
	ASSERT(boolOrder->order->type == SATC_TOTAL);
	if (boolOrder->order->encoding.resolver == NULL) {
		//This is pairwised encoding ...
		boolOrder->order->setOrderResolver(new OrderPairResolver(solver, boolOrder->order));
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
#ifdef CONFIG_DEBUG
	model_print("in total order ...\n");
#endif
	ASSERT(order->type == SATC_TOTAL);
	Set *set = order->set;
	uint size = order->set->getSize();
	for (uint i = 0; i < size; i++) {
		uint64_t valueI = set->getMemberAt(i);
		for (uint j = i + 1; j < size; j++) {
			uint64_t valueJ = set->getMemberAt(j);
			OrderPair pairIJ(valueI, valueJ, E_NULL);
			Edge constIJ = getPairConstraint(order, &pairIJ);
			for (uint k = j + 1; k < size; k++) {
				uint64_t valueK = set->getMemberAt(k);
				OrderPair pairJK(valueJ, valueK, E_NULL);
				OrderPair pairIK(valueI, valueK, E_NULL);
				Edge constIK = getPairConstraint(order, &pairIK);
				Edge constJK = getPairConstraint(order, &pairJK);
				addConstraintCNF(cnf, generateTransOrderConstraintSATEncoder(constIJ, constJK, constIK));
			}
		}
	}
}

Edge SATEncoder::generateTransOrderConstraintSATEncoder(Edge constIJ,Edge constJK,Edge constIK) {
	Edge carray[] = {constIJ, constJK, constraintNegate(constIK)};
	Edge loop1 = constraintOR(cnf, 3, carray);
	Edge carray2[] = {constraintNegate(constIJ), constraintNegate(constJK), constIK};
	Edge loop2 = constraintOR(cnf, 3, carray2 );
	return constraintAND2(cnf, loop1, loop2);
}

Edge SATEncoder::encodePartialOrderSATEncoder(BooleanOrder *constraint) {
	ASSERT(constraint->order->type == SATC_PARTIAL);
	return E_BOGUS;
}
