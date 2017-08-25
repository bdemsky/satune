#ifndef ORDER_H
#define ORDER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "ops.h"
#include "orderencoding.h"
#include "boolean.h"

class Order {
 public:
	Order(OrderType type, Set *set);
	~Order();
	OrderType type;
	Set *set;
	HashTableOrderPair *orderPairTable;
	HashSetOrderElement* elementTable;
	OrderGraph *graph;
	VectorBooleanOrder constraints;
	OrderEncoding order;
	void initializeOrderHashTable();
	void initializeOrderElementsHashTable();
	void addOrderConstraint(BooleanOrder *constraint);
	void setOrderEncodingType(OrderEncodingType type);
	MEMALLOC;
};

#endif
