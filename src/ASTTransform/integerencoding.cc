#include "integerencoding.h"
#include "orderelement.h"
#include "order.h"
#include "satencoder.h"
#include "csolver.h"
#include "predicate.h"
#include "element.h"
#include "rewriter.h"
#include "set.h"


void orderIntegerEncodingSATEncoder(SATEncoder *This, BooleanOrder *boolOrder){
	Order* order = boolOrder->order;
	if (order->elementTable == NULL) {
		order->initializeOrderElementsHashTable();
	}
	//getting two elements and using LT predicate ...
	ElementSet* elem1 = (ElementSet*)getOrderIntegerElement(This, order, boolOrder->first);
	ElementSet* elem2 = (ElementSet*)getOrderIntegerElement(This, order, boolOrder->second);
	Set * sarray[]={elem1->set, elem2->set};
	Predicate *predicate =This->solver->createPredicateOperator(LT, sarray, 2);
	Element * parray[]={elem1, elem2};
	Boolean * boolean=This->solver->applyPredicate(predicate, parray, 2);
	This->solver->addConstraint(boolean);
	This->solver->replaceBooleanWithBoolean(boolOrder, boolean);
}


Element* getOrderIntegerElement(SATEncoder* This,Order *order, uint64_t item) {
	HashSetOrderElement* eset = order->elementTable;
	OrderElement oelement(item, NULL);
	if( !eset->contains(&oelement)){
		Set* set = This->solver->createRangeSet(order->set->type, 1, (uint64_t) order->set->getSize());
		Element* elem = This->solver->getElementVar(set);
		eset->add(new OrderElement(item, elem));
		return elem;
	} else
		return eset->get(&oelement)->elem;
}

