
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

class DecomposeOrderResolver : public OrderResolver {
public:
	DecomposeOrderResolver(OrderGraph *graph, Vector<Order *> &orders);
	bool resolveOrder(uint64_t first, uint64_t second);
	bool resolvePartialOrder(OrderNode *first, OrderNode *second);
	virtual ~DecomposeOrderResolver();
private:
	OrderGraph *graph;
	Vector<Order *> orders;
};

#endif/* DECOMPOSEORDERRESOLVER_H */

