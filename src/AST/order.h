#ifndef ORDER_H
#define ORDER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "ops.h"
#include "orderencoding.h"
#include "boolean.h"

struct Order {
	OrderType type;
	Set * set;
	HashTableBoolConst* boolsToConstraints;
	VectorBoolean constraints;
	OrderEncoding order;
};

Order* allocOrder(OrderType type, Set * set);
void addOrderConstraint(Order* order, BooleanOrder* constraint);
void setOrderEncodingType(Order* order, OrderEncodingType type);
void deleteOrder(Order* order);
#endif
