#include "integerencoding.h"
#include "set.h"
#include "order.h"
#include "satencoder.h"
#include "csolver.h"
#include "integerencodingrecord.h"
#include "integerencorderresolver.h"
#include "tunable.h"

IntegerEncodingTransform::IntegerEncodingTransform(CSolver *_solver)
	: Transform(_solver)
{
	orderIntEncoding = new HashTableOrderIntEncoding();
}

IntegerEncodingTransform::~IntegerEncodingTransform() {
	orderIntEncoding->resetanddelete();
}

void IntegerEncodingTransform::doTransform() {
	HashsetOrder *orders = solver->getActiveOrders()->copy();
	SetIteratorOrder * orderit=orders->iterator();
	while(orderit->hasNext()) {
		Order *order = orderit->next();
		if (GETVARTUNABLE(solver->getTuner(), order->type, ORDERINTEGERENCODING, &onoff))
			integerEncode(order);
	}
	delete orders;
	delete orderit;
}

void IntegerEncodingTransform::integerEncode(Order *currOrder) {
	IntegerEncodingRecord *encodingRecord = NULL;
	if (!orderIntEncoding->contains(currOrder)) {
		encodingRecord = new IntegerEncodingRecord(
			solver->createRangeSet(currOrder->set->getType(), 0, (uint64_t) currOrder->set->getSize() - 1));
		orderIntEncoding->put(currOrder, encodingRecord);
		currOrder->setOrderEncodingType( INTEGERENCODING );
	} else {
		encodingRecord = orderIntEncoding->get(currOrder);
	}
	uint size = currOrder->constraints.getSize();
	for (uint i = 0; i < size; i++) {
		orderIntegerEncodingSATEncoder(currOrder, currOrder->constraints.get(i));
	}
	currOrder->setOrderResolver(new IntegerEncOrderResolver(solver, encodingRecord));
	solver->getActiveOrders()->remove(currOrder);
}


void IntegerEncodingTransform::orderIntegerEncodingSATEncoder(Order * currOrder, BooleanOrder *boolOrder) {
	IntegerEncodingRecord *ierec = orderIntEncoding->get(currOrder);
	//getting two elements and using LT predicate ...
	Element *elem1 = ierec->getOrderIntegerElement(solver, boolOrder->first);
	Element *elem2 = ierec->getOrderIntegerElement(solver, boolOrder->second);
	Set *sarray[] = {ierec->getSecondarySet(), ierec->getSecondarySet()};
	Predicate *predicate = solver->createPredicateOperator(SATC_LT, sarray, 2);
	Element *parray[] = {elem1, elem2};
	BooleanEdge boolean = solver->applyPredicate(predicate, parray, 2);
	solver->addConstraint(boolean);
	solver->replaceBooleanWithBoolean(boolOrder, boolean);
}

