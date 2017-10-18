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
	OrderPair *tmp;
	if (!(table->contains(pair))) {
		tmp = new OrderPair(pair->first, pair->second, getNewVarSATEncoder());
		table->put(tmp, tmp);
	} else {
		tmp = table->get(pair);
	}
	return negate ? tmp->getNegatedConstraint() : tmp->getConstraint();
}

Edge SATEncoder::getPartialPairConstraint(Order *order, OrderPair *pair) {
	Edge gvalue = inferOrderConstraintFromGraph(order, pair->first, pair->second);
	if (!edgeIsNull(gvalue))
		return gvalue;

	HashtableOrderPair *table = order->getOrderPairTable();

	OrderPair *tmp;
	if (!(table->contains(pair))) {
		Edge constraint = getNewVarSATEncoder();
		tmp = new OrderPair(pair->first, pair->second, constraint);
		table->put(tmp, tmp);
		Edge constraint2 = getNewVarSATEncoder();
		OrderPair *swap = new OrderPair(pair->second, pair->first, constraint2);
		table->put(swap, swap);
		addConstraintCNF(cnf, constraintNegate(constraintAND2(cnf, constraint, constraint2)));
	} else {
		tmp = table->get(pair);
	}
	return tmp->getConstraint();
}

Edge SATEncoder::encodeTotalOrderSATEncoder(BooleanOrder *boolOrder) {
	ASSERT(boolOrder->order->type == SATC_TOTAL);
	if (boolOrder->order->encoding.resolver == NULL) {
		//This is pairwised encoding ...
		boolOrder->order->setOrderResolver(new OrderPairResolver(solver, boolOrder->order));
		bool doOptOrderStructure = GETVARTUNABLE(solver->getTuner(), boolOrder->order->type, OPTIMIZEORDERSTRUCTURE, &onoff);
		if (doOptOrderStructure) {
			ASSERT(boolOrder->order->graph == NULL);
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

Edge SATEncoder::generatePartialOrderConstraintsSATEncoder(Edge ij,Edge ji, Edge jk, Edge kj,Edge ik, Edge ki) {
	Edge uoIJ = constraintAND2(cnf, constraintNegate(ij), constraintNegate(ji));
	Edge uoJK = constraintAND2(cnf, constraintNegate(jk), constraintNegate(kj));
	Edge uoIK = constraintAND2(cnf, constraintNegate(ik), constraintNegate(ki));

	Edge t1[] = {ij, jk, ik};
	Edge t2[] = {ji, jk, ik};
	Edge t3[] = {ij, kj, ki};
	Edge t4[] = {ij, kj, ik};
	Edge t5[] = {ji, jk, ki};
	Edge t6[] = {ji, kj, ki};
	Edge ct1 = constraintAND(cnf, 3, t1);
	Edge ct2 = constraintAND(cnf, 3, t2);
	Edge ct3 = constraintAND(cnf, 3, t3);
	Edge ct4 = constraintAND(cnf, 3, t4);
	Edge ct5 = constraintAND(cnf, 3, t5);
	Edge ct6 = constraintAND(cnf, 3, t6);

	Edge p1[] = {uoIJ, jk, ik};
	Edge p2[] = {ij, kj, uoIK};
	Edge p3[] = {ji, uoJK, ki};
	Edge p4[] = {uoIJ, kj, ki};
	Edge p5[] = {ji, jk, uoIK};
	Edge p6[] = {ij, uoJK, ik};
	Edge cp1 = constraintAND(cnf, 3, p1);
	Edge cp2 = constraintAND(cnf, 3, p2);
	Edge cp3 = constraintAND(cnf, 3, p3);
	Edge cp4 = constraintAND(cnf, 3, p4);
	Edge cp5 = constraintAND(cnf, 3, p5);
	Edge cp6 = constraintAND(cnf, 3, p6);

	Edge o1[] = {uoIJ, uoJK, ik};
	Edge o2[] = {ij, uoJK, uoIK};
	Edge o3[] = {uoIK, jk, uoIK};
	Edge o4[] = {ji, uoJK, uoIK};
	Edge o5[] = {uoIJ, uoJK, ki};
	Edge o6[] = {uoIJ, kj, uoIK};
	Edge co1 = constraintAND(cnf, 3, o1);
	Edge co2 = constraintAND(cnf, 3, o2);
	Edge co3 = constraintAND(cnf, 3, o3);
	Edge co4 = constraintAND(cnf, 3, o4);
	Edge co5 = constraintAND(cnf, 3, o5);
	Edge co6 = constraintAND(cnf, 3, o6);

	Edge unorder [] = {uoIJ, uoJK, uoIK};
	Edge cunorder = constraintAND(cnf, 3, unorder);


	Edge res[] = {ct1,ct2,ct3,ct4,ct5,ct6,
								cp1,cp2,cp3,cp4,cp5,cp6,
								co1,co2,co3,co4,co5,co6,
								cunorder};
	return constraintOR(cnf, 19, res);
}

Edge SATEncoder::encodePartialOrderSATEncoder(BooleanOrder *boolOrder) {
	ASSERT(boolOrder->order->type == SATC_PARTIAL);
	if (boolOrder->order->encoding.resolver == NULL) {
		//This is pairwised encoding ...
		boolOrder->order->setOrderResolver(new OrderPairResolver(solver, boolOrder->order));
		bool doOptOrderStructure = GETVARTUNABLE(solver->getTuner(), boolOrder->order->type, OPTIMIZEORDERSTRUCTURE, &onoff);
		if (doOptOrderStructure) {
			boolOrder->order->graph = buildMustOrderGraph(boolOrder->order);
			reachMustAnalysis(solver, boolOrder->order->graph, true);
		}
		createAllPartialOrderConstraintsSATEncoder(boolOrder->order);
	}
	OrderPair pair(boolOrder->first, boolOrder->second, E_NULL);
	Edge constraint = getPartialPairConstraint(boolOrder->order, &pair);
	return constraint;
}


void SATEncoder::createAllPartialOrderConstraintsSATEncoder(Order *order) {
#ifdef CONFIG_DEBUG
	model_print("in partial order ...\n");
#endif
	ASSERT(order->type == SATC_TOTAL);
	Set *set = order->set;
	uint size = order->set->getSize();
	for (uint i = 0; i < size; i++) {
		uint64_t valueI = set->getMemberAt(i);
		for (uint j = i + 1; j < size; j++) {
			uint64_t valueJ = set->getMemberAt(j);
			OrderPair pairIJ(valueI, valueJ, E_NULL);
			OrderPair pairJI(valueJ, valueI, E_NULL);
			Edge constIJ = getPartialPairConstraint(order, &pairIJ);
			Edge constJI = getPartialPairConstraint(order, &pairJI);
			for (uint k = j + 1; k < size; k++) {
				uint64_t valueK = set->getMemberAt(k);
				OrderPair pairJK(valueJ, valueK, E_NULL);
				OrderPair pairIK(valueI, valueK, E_NULL);
				Edge constIK = getPartialPairConstraint(order, &pairIK);
				Edge constJK = getPartialPairConstraint(order, &pairJK);
				OrderPair pairKJ(valueK, valueJ, E_NULL);
				OrderPair pairKI(valueK, valueI, E_NULL);
				Edge constKI = getPartialPairConstraint(order, &pairKI);
				Edge constKJ = getPartialPairConstraint(order, &pairKJ);
				addConstraintCNF(cnf, generatePartialOrderConstraintsSATEncoder(constIJ, constJI,
																																				constJK, constKJ, constIK, constKI));
			}
		}
	}
}


