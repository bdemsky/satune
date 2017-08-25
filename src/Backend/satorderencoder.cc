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
	if(constraint->order->order.type == INTEGERENCODING){
		return orderIntegerEncodingSATEncoder(This, constraint);
	}
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

Edge orderIntegerEncodingSATEncoder(SATEncoder *This, BooleanOrder *boolOrder){
	if(boolOrder->order->graph == NULL){
		bool doOptOrderStructure=GETVARTUNABLE(This->solver->tuner, boolOrder->order->type,
			OPTIMIZEORDERSTRUCTURE, &onoff);
		if (doOptOrderStructure ) {
			boolOrder->order->graph = buildMustOrderGraph(boolOrder->order);
			reachMustAnalysis(This->solver, boolOrder->order->graph, true);
		}
	}
	Order* order = boolOrder->order;
	Edge gvalue = inferOrderConstraintFromGraph(order, boolOrder->first, boolOrder->second);
	if(!edgeIsNull(gvalue))
		return gvalue;
	
	if (boolOrder->order->elementTable == NULL) {
		boolOrder->order->initializeOrderElementsHashTable();
	}
	//getting two elements and using LT predicate ...
	Element* elem1 = getOrderIntegerElement(This, order, boolOrder->first);
	ElementEncoding *encoding = getElementEncoding(elem1);
	if (getElementEncodingType(encoding) == ELEM_UNASSIGNED) {
		setElementEncodingType(encoding, BINARYINDEX);
		encodingArrayInitialization(encoding);
	}
	Element* elem2 = getOrderIntegerElement(This, order, boolOrder->second);
	encoding = getElementEncoding(elem2);
	if (getElementEncodingType(encoding) == ELEM_UNASSIGNED) {
		setElementEncodingType(encoding, BINARYINDEX);
		encodingArrayInitialization(encoding);
	}
	Set * sarray[]={order->set, order->set};
	Predicate *predicate =new PredicateOperator(LT, sarray, 2);
	Element * parray[]={elem1, elem2};
	BooleanPredicate * boolean=new BooleanPredicate(predicate, parray, 2, NULL);
	setFunctionEncodingType(boolean->getFunctionEncoding(), CIRCUIT);
	{//Adding new elements and boolean/predicate to solver regarding memory management
		This->solver->allBooleans.push(boolean);
		This->solver->allPredicates.push(predicate);
		This->solver->allElements.push(elem1);
		This->solver->allElements.push(elem2);
	}
	return encodeConstraintSATEncoder(This, boolean);
}

Edge inferOrderConstraintFromGraph(Order* order, uint64_t _first, uint64_t _second){
	if (order->graph != NULL) {
		OrderGraph *graph=order->graph;
		OrderNode *first=lookupOrderNodeFromOrderGraph(graph, _first);
		OrderNode *second=lookupOrderNodeFromOrderGraph(graph, _second);
		if ((first != NULL) && (second != NULL)) {
			OrderEdge *edge=lookupOrderEdgeFromOrderGraph(graph, first, second);
			if (edge != NULL) {
				if (edge->mustPos)
					return E_True;
				else if (edge->mustNeg)
					return E_False;
			}
			OrderEdge *invedge=getOrderEdgeFromOrderGraph(graph, second, first);
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

Element* getOrderIntegerElement(SATEncoder* This,Order *order, uint64_t item) {
	HashSetOrderElement* eset = order->elementTable;
	OrderElement oelement ={item, NULL};
	if( !eset->contains(&oelement)){
		Element* elem = new ElementSet(order->set);
		ElementEncoding* encoding = getElementEncoding(elem);
		setElementEncodingType(encoding, BINARYINDEX);
		encodingArrayInitialization(encoding);
		encodeElementSATEncoder(This, elem);
		eset->add(allocOrderElement(item, elem));
		return elem;
	} else
		return eset->get(&oelement)->elem;
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
		OrderPair *paircopy = allocOrderPair(pair->first, pair->second, constraint);
		table->put(paircopy, paircopy);
	} else
		constraint = table->get(pair)->constraint;
	
	return negate ? constraintNegate(constraint) : constraint;
}

Edge encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder *boolOrder) {
	ASSERT(boolOrder->order->type == TOTAL);
	if (boolOrder->order->orderPairTable == NULL) {
		boolOrder->order->initializeOrderHashTable();
		bool doOptOrderStructure=GETVARTUNABLE(This->solver->tuner, boolOrder->order->type, OPTIMIZEORDERSTRUCTURE, &onoff);
		if (doOptOrderStructure) {
			boolOrder->order->graph = buildMustOrderGraph(boolOrder->order);
			reachMustAnalysis(This->solver, boolOrder->order->graph, true);
		}
		createAllTotalOrderConstraintsSATEncoder(This, boolOrder->order);
	}
	OrderPair pair = {boolOrder->first, boolOrder->second, E_NULL};
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
			OrderPair pairIJ = {valueI, valueJ};
			Edge constIJ = getPairConstraint(This, order, &pairIJ);
			for (uint k = j + 1; k < size; k++) {
				uint64_t valueK = mems->get(k);
				OrderPair pairJK = {valueJ, valueK};
				OrderPair pairIK = {valueI, valueK};
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
