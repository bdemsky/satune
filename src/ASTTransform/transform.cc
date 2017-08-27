/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   transform.cc
 * Author: hamed
 * 
 * Created on August 26, 2017, 5:14 PM
 */

#include "transform.h"
#include "set.h"
#include "order.h"
#include "satencoder.h"
#include "csolver.h"
#include "integerencodingrecord.h"

Transform::Transform() {
	orderIntegerEncoding = new HashTableOrderIntegerEncoding;
}

void Transform:: orderIntegerEncodingSATEncoder(CSolver *This, BooleanOrder *boolOrder) {
	Order *order = boolOrder->order;
	if (!orderIntegerEncoding->contains(order)) {
		orderIntegerEncoding->put(order, new IntegerEncodingRecord(
		This->createRangeSet(order->set->type, 0, (uint64_t) order->set->getSize()-1)));
	}
	IntegerEncodingRecord* ierec = orderIntegerEncoding->get(order);
	//getting two elements and using LT predicate ...
	Element *elem1 = ierec->getOrderIntegerElement(This, boolOrder->first);
	Element *elem2 = ierec->getOrderIntegerElement(This, boolOrder->second);
	Set *sarray[] = {ierec->set, ierec->set};
	Predicate *predicate = This->createPredicateOperator(LT, sarray, 2);
	Element *parray[] = {elem1, elem2};
	Boolean *boolean = This->applyPredicate(predicate, parray, 2);
	This->addConstraint(boolean);
	This->replaceBooleanWithBoolean(boolOrder, boolean);
}

Transform::~Transform(){
	delete orderIntegerEncoding;
}
