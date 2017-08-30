
#include "integerencoding.h"
#include "set.h"
#include "order.h"
#include "satencoder.h"
#include "csolver.h"
#include "integerencodingrecord.h"

HashTableOrderIntegerEncoding* IntegerEncodingTransform::orderIntegerEncoding = new HashTableOrderIntegerEncoding();

IntegerEncodingTransform::IntegerEncodingTransform(CSolver* _solver, Order* _order) 
	:Transform(_solver),
	order(_order)
	
{	
}

IntegerEncodingTransform::~IntegerEncodingTransform(){
}

bool IntegerEncodingTransform::canExecuteTransform(){
	return canExecutePass(solver, order->type, ORDERINTEGERENCODING, &offon);
}

void IntegerEncodingTransform::doTransform(){
	if (!orderIntegerEncoding->contains(order)) {
		orderIntegerEncoding->put(order, new IntegerEncodingRecord(
		solver->createRangeSet(order->set->getType(), 0, (uint64_t) order->set->getSize()-1)));
	}
	uint size = order->constraints.getSize();
	for(uint i=0; i<size; i++){
		orderIntegerEncodingSATEncoder(order->constraints.get(i));
	}
}


void IntegerEncodingTransform::orderIntegerEncodingSATEncoder(BooleanOrder *boolOrder) {
	IntegerEncodingRecord* ierec = orderIntegerEncoding->get(order);
	//getting two elements and using SATC_LT predicate ...
	Element *elem1 = ierec->getOrderIntegerElement(solver, boolOrder->first);
	Element *elem2 = ierec->getOrderIntegerElement(solver, boolOrder->second);
	Set *sarray[] = {ierec->set, ierec->set};
	Predicate *predicate = solver->createPredicateOperator(SATC_LT, sarray, 2);
	Element *parray[] = {elem1, elem2};
	Boolean *boolean = solver->applyPredicate(predicate, parray, 2);
	solver->addConstraint(boolean);
	solver->replaceBooleanWithBoolean(boolOrder, boolean);
}

