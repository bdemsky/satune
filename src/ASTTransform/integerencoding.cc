
#include "integerencoding.h"
#include "set.h"
#include "order.h"
#include "satencoder.h"
#include "csolver.h"
#include "integerencodingrecord.h"


IntegerEncodingTransform::IntegerEncodingTransform(CSolver* _solver) 
	:Transform(_solver)
{	
	orderIntEncoding = new HashTableOrderIntEncoding();
}

IntegerEncodingTransform::~IntegerEncodingTransform(){
	orderIntEncoding->resetanddelete();
}

bool IntegerEncodingTransform::canExecuteTransform(){
	return canExecutePass(solver, currOrder->type, ORDERINTEGERENCODING, &offon);
}

void IntegerEncodingTransform::doTransform(){
	if (!orderIntEncoding->contains(currOrder)) {
		orderIntEncoding->put(currOrder, new IntegerEncodingRecord(
		solver->createRangeSet(currOrder->set->getType(), 0, (uint64_t) currOrder->set->getSize()-1)));
	}
	uint size = currOrder->constraints.getSize();
	for(uint i=0; i<size; i++){
		orderIntegerEncodingSATEncoder(currOrder->constraints.get(i));
	}
}


void IntegerEncodingTransform::orderIntegerEncodingSATEncoder(BooleanOrder *boolOrder) {
	IntegerEncodingRecord* ierec = orderIntEncoding->get(currOrder);
	//getting two elements and using LT predicate ...
	Element *elem1 = ierec->getOrderIntegerElement(solver, boolOrder->first);
	Element *elem2 = ierec->getOrderIntegerElement(solver, boolOrder->second);
	Set *sarray[] = {ierec->getSecondarySet(), ierec->getSecondarySet()};
	Predicate *predicate = solver->createPredicateOperator(LT, sarray, 2);
	Element *parray[] = {elem1, elem2};
	Boolean *boolean = solver->applyPredicate(predicate, parray, 2);
	solver->addConstraint(boolean);
	solver->replaceBooleanWithBoolean(boolOrder, boolean);
}

