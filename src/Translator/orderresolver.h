
/*
 * File:   orderresolver.h
 * Author: hamed
 *
 * Created on August 31, 2017, 7:16 PM
 */

#ifndef ORDERRESOLVER_H
#define ORDERRESOLVER_H
#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

class OrderResolver {
public:
	OrderResolver() {};
	virtual HappenedBefore resolveOrder(uint64_t first, uint64_t second) = 0;
	virtual ~OrderResolver() {};
	CMEMALLOC;
};

#endif/* ORDERRESOLVER_H */

