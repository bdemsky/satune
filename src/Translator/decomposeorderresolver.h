
/*
 * File:   DecomposeOrderResolver.h
 * Author: hamed
 *
 * Created on September 1, 2017, 10:36 AM
 */

#ifndef DECOMPOSEORDERRESOLVER_H
#define DECOMPOSEORDERRESOLVER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "orderresolver.h"

class DOREdge {
public:
	DOREdge(uint64_t _origfirst, uint64_t _origsecond, uint _orderindex, uint64_t _newfirst, uint64_t _newsecond) :
		origfirst(_origfirst),
		origsecond(_origsecond),
		orderindex(_orderindex),
		newfirst(_newfirst),
		newsecond(_newsecond),
		mustbetrue(false) {
	}
	uint64_t origfirst;
	uint64_t origsecond;
	uint orderindex;
	uint64_t newfirst;
	uint64_t newsecond;
	bool mustbetrue;
	CMEMALLOC;
};

class DecomposeOrderResolver : public OrderResolver {
public:
	DecomposeOrderResolver(Order *_order);
	virtual bool resolveOrder(uint64_t first, uint64_t second);
	virtual ~DecomposeOrderResolver();
	void mustOrderEdge(uint64_t first, uint64_t second);
	void remapEdge(uint64_t oldfirst, uint64_t oldsecond, uint64_t newfirst, uint64_t newsecond);
	void setEdgeOrder(uint64_t first, uint64_t second, uint sccNum);
	void setOrder(uint sccNum, Order *order);
	Order *getOrder(uint sccNum);
	CMEMALLOC;

private:
	bool resolvePartialOrder(OrderNode *first, OrderNode *second);
	void buildGraph();
	OrderGraph *graph;
	Order *order;
	Vector<Order *> orders;
	HashsetDOREdge edges;
};

#endif/* DECOMPOSEORDERRESOLVER_H */

