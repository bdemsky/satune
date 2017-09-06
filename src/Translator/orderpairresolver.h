
/*
 * File:   orderpairresolver.h
 * Author: hamed
 *
 * Created on September 1, 2017, 3:36 PM
 */

#ifndef ORDERPAIRRESOLVER_H
#define ORDERPAIRRESOLVER_H

#include "orderresolver.h"

class OrderPairResolver : public OrderResolver {
public:
	OrderPairResolver(CSolver *_solver, Order *_order);
	bool resolveOrder(uint64_t first, uint64_t second);
	HashtableOrderPair* getOrderPairTable() { return orderPairTable;}
	virtual ~OrderPairResolver();
private:
	CSolver *solver;
	Order *order;
	HashtableOrderPair *orderPairTable;
	
	bool getOrderConstraintValue(uint64_t first, uint64_t second);
};

#endif/* ORDERPAIRRESOLVER_H */

