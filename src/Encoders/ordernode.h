
/* 
 * File:   ordernode.h
 * Author: hamed
 *
 * Created on August 7, 2017, 3:43 PM
 */

#ifndef ORDERNODE_H
#define ORDERNODE_H

#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "orderedge.h"
struct OrderNode{
	uint64_t id;
	Order* order;
	HashSetOrderEdge* inEdges;
	HashSetOrderEdge* outEdges;
};

OrderNode* allocOrderNode(uint64_t id, Order* order);
void addNewIncomingEdge(OrderNode* node, OrderEdge* edge);
void addNewOutgoingEdge(OrderNode* node, OrderEdge* edge);
void deleteOrderNode(OrderNode* node);

#endif /* ORDERNODE_H */

