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
	HashSetOrderEdge* edges;
	VectorOrderNode scc;
};

OrderGraph* allocOrderGraph();
void addOrderConstraintToOrderGraph(OrderGraph* graph, Boolean* constr);
OrderNode* getOrderNodeFromOrderGraph(OrderGraph* graph, uint64_t id);
OrderEdge* getOrderEdgeFromOrderGraph(OrderGraph* graph, Boolean* order, OrderNode* begin, OrderNode* end);
void addOrderEdge(OrderGraph* graph, OrderNode* node1, OrderNode* node2, Boolean* constr);
void deleteOrderGraph(OrderGraph* graph);

#endif /* ORDERGRAPH_H */

