#ifndef ORDER_H
#define ORDER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "ops.h"
struct Order {
	OrderType type;
	Set * set;
	VectorBoolean* constraints;
};

Order* allocOrder(OrderType type, Set * set);
Boolean* getOrderConstraint(Order* order,uint64_t first, uint64_t second);
void deleteOrder(Order* order);

#endif
