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

class IntegerEncodingTransform : public Transform{
public:
	IntegerEncodingTransform(CSolver* solver, Order* order);
	void orderIntegerEncodingSATEncoder(BooleanOrder *boolOrder);
	void doTransform();
	bool canExecuteTransform();
	virtual ~IntegerEncodingTransform();
private:
	Order* order;
	// In future we can use a singleton class instead of static variable for keeping data that needed
	// for translating back result
	static HashtableOrderIntegerEncoding* orderIntegerEncoding;
};


#endif /* INTEGERENCODING_H */

