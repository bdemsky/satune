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

class OrderGraph {
public:
	OrderGraph(Order *order);
	~OrderGraph();
	void addOrderConstraintToOrderGraph(BooleanOrder *bOrder);
	void addMustOrderConstraintToOrderGraph(BooleanOrder *bOrder);
	OrderNode *getOrderNodeFromOrderGraph(uint64_t id);
	OrderEdge *getOrderEdgeFromOrderGraph(OrderNode *begin, OrderNode *end);
	OrderNode *lookupOrderNodeFromOrderGraph(uint64_t id);
	OrderEdge *lookupOrderEdgeFromOrderGraph(OrderNode *begin, OrderNode *end);
	void addOrderEdge(OrderNode *node1, OrderNode *node2, BooleanOrder *constr);
	void addMustOrderEdge(OrderNode *node1, OrderNode *node2, BooleanOrder *constr);
	OrderEdge *getInverseOrderEdge(OrderEdge *edge);
	Order *getOrder() {return order;}
	HSIteratorOrderNode *getNodes() {return nodes->iterator();}
	HSIteratorOrderEdge *getEdges() {return edges->iterator();}

	CMEMALLOC;
private:
	HashSetOrderNode *nodes;
	HashSetOrderEdge *edges;
	Order *order;
};

OrderGraph *buildOrderGraph(Order *order);
OrderGraph *buildMustOrderGraph(Order *order);
#endif/* ORDERGRAPH_H */

