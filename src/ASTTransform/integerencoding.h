/*
 * File:   integerencoding.h
 * Author: hamed
 *
 * Created on August 27, 2017, 4:36 PM
 */

#ifndef INTEGERENCODING_H
#define INTEGERENCODING_H
#include "classlist.h"
#include "transform.h"
#include "order.h"

class IntegerEncodingTransform : public Transform {
public:
	IntegerEncodingTransform(CSolver *solver);
	void orderIntegerEncodingSATEncoder(BooleanOrder *boolOrder);
	void doTransform();
	void integerEncode(Order *currOrder);

	virtual ~IntegerEncodingTransform();
private:
	Order *currOrder;
	//FIXME:We can remove it, because we don't need it for translating anymore... -HG
	HashTableOrderIntEncoding *orderIntEncoding;
};


#endif/* INTEGERENCODING_H */

