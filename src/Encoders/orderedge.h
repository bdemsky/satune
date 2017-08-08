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

struct OrderEdge{
	Boolean* order;
	OrderNode* source;
	OrderNode* sink;
};

OrderEdge* allocOrderEdge(Boolean* order, OrderNode* begin, OrderNode* end);
void deleteOrderEdge(OrderEdge* This);
#endif /* ORDEREDGE_H */

