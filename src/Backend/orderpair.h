/* 
 * File:   orderpair.h
 * Author: hamed
 *
 * Created on July 1, 2017, 4:22 PM
 */

#ifndef ORDERPAIR_H
#define ORDERPAIR_H

#include "classlist.h"
#include "mymemory.h"
#include "constraint.h"

struct OrderPair{
	uint64_t first;
	uint64_t second;
	Edge constraint;
}; 

OrderPair* allocOrderPair(uint64_t first, uint64_t second, Edge constraint);
void deleteOrderPair(OrderPair* pair);

#endif /* ORDERPAIR_H */

