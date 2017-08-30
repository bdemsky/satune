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
	set(_set)
{
	elementTable = new HashSetOrderElement();
}

IntegerEncodingRecord::~IntegerEncodingRecord(){
	if (elementTable != NULL) {
		delete elementTable;
	}
}

Element * IntegerEncodingRecord::getOrderIntegerElement(CSolver *This, uint64_t item) {
	OrderElement oelement(item, NULL);
	if ( !elementTable->contains(&oelement)) {
		Element *elem = This->getElementVar(set);
		elementTable->add(new OrderElement(item, elem));
		return elem;
	} else
		return elementTable->get(&oelement)->elem;
}

