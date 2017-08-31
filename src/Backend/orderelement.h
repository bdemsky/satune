/*
 * File:   orderelement.h
 * Author: hamed
 *
 * Created on July 1, 2017, 4:22 PM
 */

#ifndef ORDERELEMENT_H
#define ORDERELEMENT_H

#include "classlist.h"
#include "mymemory.h"
#include "constraint.h"

class OrderElement {
public:
	OrderElement(uint64_t item, Element *elem);
	inline uint getHash() {return (uint) item;}
	inline bool equals(OrderElement* oe){ return item == oe->item;}
	inline Element* getElement() { return elem; }
	MEMALLOC;
private:
	uint64_t item;
	Element *elem;
};


#endif/* ORDERELEMENT_H */

