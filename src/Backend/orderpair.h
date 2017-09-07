/*
 * File:   orderpair.h
 * Author: hamed
 *
 * Created on July 1, 2017, 4:22 PM
 */

#ifndef ORDERPAIR_H
#define ORDERPAIR_H

#include "classlist.h"
#include "mymemory.h"
#include "constraint.h"

class OrderPair {
public:
	OrderPair(uint64_t first, uint64_t second, Edge constraint = E_NULL);
	OrderPair();
	virtual ~OrderPair();
	virtual Edge getConstraint();
	virtual bool getConstraintValue(CSolver* solver);
	//for the cases that we swap first and second ... For total order is straight forward.
	// but for partial order it has some complexity which should be hidden ... -HG
	virtual Edge getNegatedConstraint();
	virtual bool getNegatedConstraintValue(CSolver* solver);
	uint64_t first;
	uint64_t second;
	CMEMALLOC;
protected:
	Edge constraint;
};

#endif/* ORDERPAIR_H */

