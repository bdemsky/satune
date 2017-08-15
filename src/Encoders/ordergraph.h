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

struct OrderGraph {
	HashSetOrderNode* nodes;
	HashSetOrderEdge* edges;
	Order* order;
	VectorOrderNode scc;
};

OrderGraph* allocOrderGraph(Order *order);
void addOrderConstraintToOrderGraph(OrderGraph* graph, Boolean* constr);
OrderNode* getOrderNodeFromOrderGraph(OrderGraph* graph, uint64_t id);
OrderEdge* getOrderEdgeFromOrderGraph(OrderGraph* graph, OrderNode* begin, OrderNode* end);
void addOrderEdge(OrderGraph* graph, OrderNode* node1, OrderNode* node2, Boolean* constr);
void deleteOrderGraph(OrderGraph* graph);
OrderEdge* getInverseOrderEdge(OrderGraph* graph, OrderEdge *edge);
#endif /* ORDERGRAPH_H */

