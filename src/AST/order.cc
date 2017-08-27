#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"
#include "ordergraph.h"

Order::Order(OrderType _type, Set *_set) :
	type(_type),
	set(_set),
	orderPairTable(NULL),
	graph(NULL),
	order(this)
{
}

void Order::initializeOrderHashTable() {
	orderPairTable = new HashTableOrderPair();
}


void Order::addOrderConstraint(BooleanOrder *constraint) {
	constraints.push(constraint);
}

void Order::setOrderEncodingType(OrderEncodingType type) {
	order.type = type;
}

Order::~Order() {
	if (orderPairTable != NULL) {
		orderPairTable->resetanddelete();
		delete orderPairTable;
	}
	
	if (graph != NULL) {
		delete graph;
	}
}
