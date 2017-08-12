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
#include "ordernode.h"

struct OrderEdge {
	OrderNode* source;
	OrderNode* sink;
};

OrderEdge* allocOrderEdge(OrderNode* begin, OrderNode* end);
void deleteOrderEdge(OrderEdge* This);

#endif /* ORDEREDGE_H */

