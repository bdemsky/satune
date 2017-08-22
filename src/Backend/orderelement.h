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

struct OrderElement {
	uint64_t item;
	Element* elem;
};

OrderElement *allocOrderElement(uint64_t item, Element* elem);
void deleteOrderElement(OrderElement *pair);

#endif/* ORDERELEMENT_H */

