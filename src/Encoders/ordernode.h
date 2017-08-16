
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

enum NodeStatus {NOTVISITED, VISITED, FINISHED};
typedef enum NodeStatus NodeStatus;

struct OrderNode {
	uint64_t id;
	HashSetOrderEdge* inEdges;
	HashSetOrderEdge* outEdges;
	NodeStatus status;
	uint sccNum;
};

OrderNode* allocOrderNode(uint64_t id);
void addNewIncomingEdge(OrderNode* node, OrderEdge* edge);
void addNewOutgoingEdge(OrderNode* node, OrderEdge* edge);
void deleteOrderNode(OrderNode* node);

#endif /* ORDERNODE_H */

