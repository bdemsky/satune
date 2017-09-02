
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


HappenedBefore IntegerEncOrderResolver::resolveOrder(uint64_t first, uint64_t second) {
	Element *elem1 = ierecord->getOrderIntegerElement(solver, first, false);
	if (elem1 == NULL)
		return SATC_UNORDERED;
	Element *elem2 = ierecord->getOrderIntegerElement(solver, second, false);
	if (elem2 == NULL)
		return SATC_UNORDERED;

	uint64_t val1 = getElementValueSATTranslator(solver, elem1);
	uint64_t val2 = getElementValueSATTranslator(solver, elem2);
	return val1 < val2 ? SATC_FIRST : val1> val2 ? SATC_SECOND : SATC_UNORDERED;
}
