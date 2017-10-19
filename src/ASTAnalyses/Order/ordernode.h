
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

enum NodeStatus {NOTVISITED, VISITED, FINISHED, ADDEDTOSET};
typedef enum NodeStatus NodeStatus;

class OrderNode {
public:
	OrderNode(uint64_t id);
	void addNewIncomingEdge(OrderEdge *edge);
	void addNewOutgoingEdge(OrderEdge *edge);
	uint64_t getID() {return id;}

	uint64_t id;
	NodeStatus status;
	uint sccNum;
	HashsetOrderEdge inEdges;
	HashsetOrderEdge outEdges;
	CMEMALLOC;
};
#endif/* ORDERNODE_H */

