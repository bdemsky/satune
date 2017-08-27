/*
 * File:   orderedge.h
 * Author: hamed
 *
 * Created on August 7, 2017, 3:44 PM
 */

#ifndef ORDEREDGE_H
#define ORDEREDGE_H
#include "classlist.h"
#include "mymemory.h"

class OrderEdge {
 public:
	OrderEdge(OrderNode *begin, OrderNode *end);

	OrderNode *source;
	OrderNode *sink;
	unsigned int polPos : 1;
	unsigned int polNeg : 1;
	unsigned int mustPos : 1;
	unsigned int mustNeg : 1;
	unsigned int pseudoPos : 1;
	MEMALLOC;
};

#endif/* ORDEREDGE_H */

