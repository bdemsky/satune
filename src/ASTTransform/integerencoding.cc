#include "integerencoding.h"
#include "set.h"
#include "order.h"
#include "satencoder.h"
#include "csolver.h"
#include "integerencodingrecord.h"
#include "integerencorderresolver.h"
#include "tunable.h"
#include "polarityassignment.h"

IntegerEncodingTransform::IntegerEncodingTransform(CSolver *_solver)
	: Transform(_solver)
{
}

IntegerEncodingTransform::~IntegerEncodingTransform() {
}

void IntegerEncodingTransform::doTransform() {
	if (solver->isUnSAT()) {
		return;
	}
	HashsetOrder *orders = solver->getActiveOrders()->copy();
	SetIteratorOrder *orderit = orders->iterator();
	while (orderit->hasNext()) {
		Order *order = orderit->next();
		if (GETVARTUNABLE(solver->getTuner(), order->type, ORDERINTEGERENCODING, &offon))
			integerEncode(order);
	}
	delete orders;
	delete orderit;
}

void IntegerEncodingTransform::integerEncode(Order *currOrder) {
	IntegerEncodingRecord *encodingRecord =  new IntegerEncodingRecord(
		solver->createRangeSet(currOrder->set->getType(), 0, (uint64_t) currOrder->getNumUsed() - 1));
	currOrder->setOrderEncodingType( INTEGERENCODING );

	Vector<BooleanOrder *> *constraints = currOrder->getConstraints();
	uint size = constraints->getSize();
	for (uint i = 0; i < size; i++) {
		orderIntegerEncodingSATEncoder(constraints->get(i), encodingRecord);
	}
	currOrder->setOrderResolver(new IntegerEncOrderResolver(solver, encodingRecord));
	solver->getActiveOrders()->remove(currOrder);
}


void IntegerEncodingTransform::orderIntegerEncodingSATEncoder(BooleanOrder *boolOrder, IntegerEncodingRecord *ierec) {
	//getting two elements and using LT predicate ...
	Element *elem1 = ierec->getOrderIntegerElement(solver, boolOrder->first);
	Element *elem2 = ierec->getOrderIntegerElement(solver, boolOrder->second);
	Predicate *predicate = solver->createPredicateOperator(SATC_LT);
	Element *parray[] = {elem1, elem2};
	BooleanEdge boolean = solver->applyPredicate(predicate, parray, 2);
	updateEdgePolarity(boolean, boolOrder);
	solver->replaceBooleanWithBoolean(boolOrder, boolean);
}

