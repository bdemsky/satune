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
	uint64_t item;
	Element *elem;
	MEMALLOC;
};


#endif/* ORDERELEMENT_H */

