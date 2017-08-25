#include "integerencoding.h"
#include "orderelement.h"
#include "order.h"
#include "satencoder.h"
#include "csolver.h"
#include "predicate.h"
#include "element.h"
#include "rewriter.h"

/*
void orderIntegerEncodingSATEncoder(SATEncoder *This, BooleanOrder *boolOrder){
	Order* order = boolOrder->order;
	if (order->elementTable == NULL) {
		order->initializeOrderElementsHashTable();
	}
	//getting two elements and using LT predicate ...
	Element* elem1 = getOrderIntegerElement(This, order, boolOrder->first);
	Element* elem2 = getOrderIntegerElement(This, order, boolOrder->second);
	Set * sarray[]={order->auxSet, order->auxSet};
	Predicate *predicate =new PredicateOperator(LT, sarray, 2);
	Element * parray[]={elem1, elem2};
	BooleanPredicate * boolean=new BooleanPredicate(predicate, parray, 2, NULL);
	{//Adding new elements and boolean/predicate to solver regarding memory management
		This->solver->allBooleans.push(boolean);
		This->solver->allPredicates.push(predicate);
		This->solver->allElements.push(elem1);
		This->solver->allElements.push(elem2);
		This->solver->constraints.add(boolean);
	}
	replaceBooleanWithBoolean(This->solver, boolOrder, boolean);
}


Element* getOrderIntegerElement(SATEncoder* This,Order *order, uint64_t item) {
	HashSetOrderElement* eset = order->elementTable;
	OrderElement oelement ={item, NULL};
	if( !eset->contains(&oelement)){
		Element* elem = new ElementSet(order->auxSet);
		eset->add(allocOrderElement(item, elem));
		return elem;
	} else
		return eset->get(&oelement)->elem;
}

*/
