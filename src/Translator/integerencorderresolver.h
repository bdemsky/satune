
/*
 * File:   integerencorderresolver.h
 * Author: hamed
 *
 * Created on September 1, 2017, 4:58 PM
 */

#ifndef INTEGERENCORDERRESOLVER_H
#define INTEGERENCORDERRESOLVER_H
#include "orderresolver.h"

class IntegerEncOrderResolver : public OrderResolver {
public:
	IntegerEncOrderResolver(CSolver *_solver, IntegerEncodingRecord *_ierecord);
	HappenedBefore resolveOrder(uint64_t first, uint64_t second);
	virtual ~IntegerEncOrderResolver();
private:
	CSolver *solver;
	IntegerEncodingRecord *ierecord;
};

#endif/* INTEGERENCORDERRESOLVER_H */

