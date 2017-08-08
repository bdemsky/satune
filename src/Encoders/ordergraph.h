/* 
 * File:   ordergraph.h
 * Author: hamed
 *
 * Created on August 7, 2017, 3:42 PM
 */

#ifndef ORDERGRAPH_H
#define ORDERGRAPH_H
#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

struct OrderGraph{
	HashSetOrderNode* nodes;
};

OrderGraph* allocOrderGraph();

void deleteOrderGraph(OrderGraph* graph);

#endif /* ORDERGRAPH_H */

