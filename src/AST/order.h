#ifndef ORDER_H
#define ORDER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "ops.h"
#include "orderencoding.h"
#include "boolean.h"
#include "orderpair.h"

class Order {
public:
	Order(OrderType type, Set *set);
	~Order();
	OrderType type;
	Set *set;
	HashtableOrderPair *orderPairTable;
	OrderGraph *graph;
	Order *clone(CSolver *solver, CloneMap *map);
	Vector<BooleanOrder *> constraints;
	OrderEncoding order;
	void initializeOrderHashtable();
	void initializeOrderElementsHashtable();
	void addOrderConstraint(BooleanOrder *constraint);
	void setOrderEncodingType(OrderEncodingType type);
	CMEMALLOC;
};

#endif
