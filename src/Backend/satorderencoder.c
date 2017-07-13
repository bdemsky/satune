#include "satencoder.h"
#include "structs.h"
#include "common.h"
#include "order.h"
#include "orderpair.h"
#include "set.h"

Edge encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint) {
	switch( constraint->order->type){
		case PARTIAL:
			return encodePartialOrderSATEncoder(This, constraint);
		case TOTAL:
			return encodeTotalOrderSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge getPairConstraint(SATEncoder *This, HashTableOrderPair * table, OrderPair * pair) {
	bool negate = false;
	OrderPair flipped;
	if (pair->first > pair->second) {
		negate=true;
		flipped.first=pair->second;
		flipped.second=pair->first;
		pair = &flipped;
	}
	Edge constraint;
	if (!containsOrderPair(table, pair)) {
		constraint = getNewVarSATEncoder(This);
		OrderPair * paircopy = allocOrderPair(pair->first, pair->second, constraint);
		putOrderPair(table, paircopy, paircopy);
	} else
		constraint = getOrderPair(table, pair)->constraint;

	return negate ? constraintNegate(constraint) : constraint;
}

Edge encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * boolOrder) {
	ASSERT(boolOrder->order->type == TOTAL);
	if(boolOrder->order->orderPairTable == NULL) {
		initializeOrderHashTable(boolOrder->order);
		createAllTotalOrderConstraintsSATEncoder(This, boolOrder->order);
	}
	HashTableOrderPair* orderPairTable = boolOrder->order->orderPairTable;
	OrderPair pair={boolOrder->first, boolOrder->second, E_NULL};
	Edge constraint = getPairConstraint(This, orderPairTable, & pair);
	return constraint;
}


void createAllTotalOrderConstraintsSATEncoder(SATEncoder* This, Order* order){
#ifdef TRACE_DEBUG
	model_print("in total order ...\n");
#endif	
	ASSERT(order->type == TOTAL);
	VectorInt* mems = order->set->members;
	HashTableOrderPair* table = order->orderPairTable;
	uint size = getSizeVectorInt(mems);
	uint csize =0;
	for(uint i=0; i<size; i++){
		uint64_t valueI = getVectorInt(mems, i);
		for(uint j=i+1; j<size;j++){
			uint64_t valueJ = getVectorInt(mems, j);
			OrderPair pairIJ = {valueI, valueJ};
			Edge constIJ=getPairConstraint(This, table, & pairIJ);
			for(uint k=j+1; k<size; k++){
				uint64_t valueK = getVectorInt(mems, k);
				OrderPair pairJK = {valueJ, valueK};
				OrderPair pairIK = {valueI, valueK};
				Edge constIK = getPairConstraint(This, table, & pairIK);
				Edge constJK = getPairConstraint(This, table, & pairJK);
				addConstraintCNF(This->cnf, generateTransOrderConstraintSATEncoder(This, constIJ, constJK, constIK)); 
			}
		}
	}
}

Edge getOrderConstraint(HashTableOrderPair *table, OrderPair *pair){
	ASSERT(pair->first!= pair->second);
	Edge constraint = getOrderPair(table, pair)->constraint;
	if(pair->first > pair->second)
		return constraint;
	else
		return constraintNegate(constraint);
}

Edge generateTransOrderConstraintSATEncoder(SATEncoder *This, Edge constIJ,Edge constJK,Edge constIK){
	Edge carray[] = {constIJ, constJK, constraintNegate(constIK)};
	Edge loop1= constraintOR(This->cnf, 3, carray);
	Edge carray2[] = {constraintNegate(constIJ), constraintNegate(constJK), constIK};
	Edge loop2= constraintOR(This->cnf, 3, carray2 );
	return constraintAND2(This->cnf, loop1, loop2);
}

Edge encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint){
	ASSERT(constraint->order->type == PARTIAL);
	return E_BOGUS;
}
