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
	void addEdge(uint64_t first, uint64_t second);
	OrderEdge *getInverseOrderEdge(OrderEdge *edge);
	Order *getOrder() {return order;}
	bool isTherePath(OrderNode *source, OrderNode *destination);
	bool isTherePathVisit(HashsetOrderNode &visited, OrderNode *current, OrderNode *destination);
	SetIteratorOrderNode *getNodes() {return nodes.iterator();}
	SetIteratorOrderEdge *getEdges() {return edges.iterator();}
	void DFS(Vector<OrderNode *> *finishNodes);
	void DFSMust(Vector<OrderNode *> *finishNodes);
	void computeStronglyConnectedComponentGraph();
	void resetNodeInfoStatusSCC();
	void completePartialOrderGraph();
	void removeNode(OrderNode *node) {nodes.remove(node);}
	
	CMEMALLOC;
private:
	HashsetOrderNode nodes;
	Vector<OrderNode *> allNodes;
	HashsetOrderEdge edges;
	Order *order;
	void DFSNodeVisit(OrderNode *node, Vector<OrderNode *> *finishNodes, bool isReverse, bool mustvisit, uint sccNum);
	void DFSReverse(Vector<OrderNode *> *finishNodes);
};

OrderGraph *buildOrderGraph(Order *order);
OrderGraph *buildMustOrderGraph(Order *order);
#endif/* ORDERGRAPH_H */

