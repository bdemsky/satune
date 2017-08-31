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
	OrderPair(uint64_t first, uint64_t second, Edge constraint);
	OrderPair();
	uint64_t first;
	uint64_t second;
	Edge constraint;
	CMEMALLOC;
};

#endif/* ORDERPAIR_H */

