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

struct OrderEdge {
	OrderNode* source;
	OrderNode* sink;
	unsigned int polPos:1;
	unsigned int polNeg:1;
	unsigned int mustPos:1;
	unsigned int mustNeg:1;
};

OrderEdge* allocOrderEdge(OrderNode* begin, OrderNode* end);
void deleteOrderEdge(OrderEdge* This);

#endif /* ORDEREDGE_H */

