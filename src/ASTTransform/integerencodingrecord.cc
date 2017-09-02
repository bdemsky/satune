/* 
 * File:   integerencodingrecord.cpp
 * Author: hamed
 * 
 * Created on August 26, 2017, 6:19 PM
 */

#include "integerencodingrecord.h"
#include "csolver.h"
#include "orderelement.h"

IntegerEncodingRecord::IntegerEncodingRecord(Set* _set):
	secondarySet(_set)
{
	elementTable = new HashsetOrderElement();
}

IntegerEncodingRecord::~IntegerEncodingRecord(){
	if (elementTable != NULL) {
		delete elementTable;
	}
}

Element * IntegerEncodingRecord::getOrderIntegerElement(CSolver *This, uint64_t item, bool create) {
	OrderElement oelement(item, NULL);
	if ( elementTable->contains(&oelement)) {
		return elementTable->get(&oelement)->getElement();
	} else if (create) {
		Element *elem = This->getElementVar(secondarySet);
		elementTable->add(new OrderElement(item, elem));
		return elem;
	}
	return NULL;
}

