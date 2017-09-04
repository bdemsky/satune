/*
 * File:   integerencodingrecord.cpp
 * Author: hamed
 *
 * Created on August 26, 2017, 6:19 PM
 */

#include "integerencodingrecord.h"
#include "csolver.h"
#include "orderelement.h"

IntegerEncodingRecord::IntegerEncodingRecord(Set *_set) :
	secondarySet(_set)
{
	elementSet = new HashsetOrderElement();
}

IntegerEncodingRecord::~IntegerEncodingRecord() {
	SetIteratorOrderElement *oiterator = elementSet->iterator();
	while (oiterator->hasNext()) {
		OrderElement *oe = oiterator->next();
		delete oe;
	}
	delete oiterator;
	delete elementSet;
}

Element *IntegerEncodingRecord::getOrderIntegerElement(CSolver *This, uint64_t item, bool create) {
	OrderElement oelement(item, NULL);
	if ( elementSet->contains(&oelement)) {
		return elementSet->get(&oelement)->getElement();
	} else if (create) {
		Element *elem = This->getElementVar(secondarySet);
		elementSet->add(new OrderElement(item, elem));
		return elem;
	}
	return NULL;
}

