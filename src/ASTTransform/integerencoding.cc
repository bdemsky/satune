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
	Predicate *predicate =new PredicateOperator(LT, sarray, 2);
	Element * parray[]={elem1, elem2};
	BooleanPredicate * boolean=new BooleanPredicate(predicate, parray, 2, NULL);
	{//Adding new elements and boolean/predicate to solver regarding memory management
		This->solver->allBooleans.push(boolean);
		This->solver->allPredicates.push(predicate);
		This->solver->constraints.add(boolean);
	}
	replaceBooleanWithBoolean(This->solver, boolOrder, boolean);
}


Element* getOrderIntegerElement(SATEncoder* This,Order *order, uint64_t item) {
	HashSetOrderElement* eset = order->elementTable;
	OrderElement oelement ={item, NULL};
	if( !eset->contains(&oelement)){
		Set* set = new Set(order->set->type, 1, (uint64_t) order->set->getSize());
		Element* elem = new ElementSet(set);
		eset->add(allocOrderElement(item, elem));
		This->solver->allElements.push(elem);
		This->solver->allSets.push(set);
		return elem;
	} else
		return eset->get(&oelement)->elem;
}

