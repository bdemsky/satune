/* 
 * File:   integerencodingrecord.h
 * Author: hamed
 *
 * Created on August 26, 2017, 6:19 PM
 */

#ifndef INTEGERENCODINGRECORD_H
#define INTEGERENCODINGRECORD_H
#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

class IntegerEncodingRecord {
public:
	IntegerEncodingRecord(Set* set);
	~IntegerEncodingRecord();
	Element* getOrderIntegerElement(CSolver *This, uint64_t item);
	inline Set* getSecondarySet() { return secondarySet; }
	MEMALLOC;
private:
	Set* secondarySet;
	HashSetOrderElement *elementTable;
};

#endif /* INTEGERENCODINGRECORD_H */

