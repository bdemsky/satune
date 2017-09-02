
/*
 * File:   integerencorderresolver.cpp
 * Author: hamed
 *
 * Created on September 1, 2017, 4:58 PM
 */

#include "integerencorderresolver.h"
#include "integerencodingrecord.h"
#include "sattranslator.h"

IntegerEncOrderResolver::IntegerEncOrderResolver(CSolver *_solver, IntegerEncodingRecord *_ierecord) :
	solver(_solver),
	ierecord(_ierecord)
{
}

IntegerEncOrderResolver::~IntegerEncOrderResolver() {
}


bool IntegerEncOrderResolver::resolveOrder(uint64_t first, uint64_t second) {
	Element *elem1 = ierecord->getOrderIntegerElement(solver, first, false);
	ASSERT (elem1 != NULL);
	Element *elem2 = ierecord->getOrderIntegerElement(solver, second, false);
	ASSERT (elem2 != NULL);

	uint64_t val1 = getElementValueSATTranslator(solver, elem1);
	uint64_t val2 = getElementValueSATTranslator(solver, elem2);
	return val1 < val2;
}
