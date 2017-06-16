#ifndef ORDER_H
#define ORDER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "ops.h"
struct Order {
	enum OrderType type;
	Set * set;
	VectorBoolean* constraints;
};

Order* allocOrder(enum OrderType type, Set * set);
Boolean* getOrderConstraint(Order* order,uint64_t first, uint64_t second);
#endif
