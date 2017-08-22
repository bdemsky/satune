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
	Set *set;
	HashTableOrderPair *orderPairTable;
	HashSetOrderElement* elementTable;
	OrderGraph *graph;
	VectorBooleanOrder constraints;
	OrderEncoding order;
};

Order *allocOrder(OrderType type, Set *set);
void initializeOrderHashTable(Order *This);
void initializeOrderElementsHashTable(Order *This);
void addOrderConstraint(Order *This, BooleanOrder *constraint);
void setOrderEncodingType(Order *This, OrderEncodingType type);
void deleteOrder(Order *This);
#endif
