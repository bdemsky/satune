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

struct OrderPair{
	uint64_t first;
	uint64_t second;
	Constraint *constraint;
}; 

OrderPair* allocOrderPair(uint64_t first, uint64_t second, Constraint * constraint);
void deleteOrderPair(OrderPair* pair);

#endif /* ORDERPAIR_H */

