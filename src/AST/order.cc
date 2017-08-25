#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"
#include "ordergraph.h"

Order::Order(OrderType _type, Set *_set) : type(_type), set(_set), orderPairTable(NULL), elementTable(NULL), graph(NULL) {
	initDefVectorBooleanOrder(&constraints);
	initOrderEncoding(&order, this);
}

void Order::initializeOrderHashTable() {
	orderPairTable = allocHashTableOrderPair(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
}

void Order::initializeOrderElementsHashTable() {
	elementTable = allocHashSetOrderElement(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
}

void Order::addOrderConstraint(BooleanOrder *constraint) {
	pushVectorBooleanOrder(&constraints, constraint);
}

void Order::setOrderEncodingType(OrderEncodingType type) {
	order.type = type;
}

Order::~Order() {
	deleteVectorArrayBooleanOrder(&constraints);
	deleteOrderEncoding(&order);
	if (orderPairTable != NULL) {
		resetAndDeleteHashTableOrderPair(orderPairTable);
		deleteHashTableOrderPair(orderPairTable);
	}
	if(elementTable != NULL){
		deleteHashSetOrderElement(elementTable);
	}
	if (graph != NULL) {
		deleteOrderGraph(graph);
	}
}
