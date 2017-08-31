#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"
#include "ordergraph.h"
#include "csolver.h"

Order::Order(OrderType _type, Set *_set) :
	type(_type),
	set(_set),
	orderPairTable(NULL),
	graph(NULL),
	order(this)
{
}

void Order::initializeOrderHashtable() {
	orderPairTable = new HashtableOrderPair();
}


void Order::addOrderConstraint(BooleanOrder *constraint) {
	constraints.push(constraint);
}

void Order::setOrderEncodingType(OrderEncodingType type) {
	order.type = type;
}

Order *Order::clone(CSolver *solver, CloneMap *map) {
	Order *o = (Order *)map->get(this);
	if (o != NULL)
		return o;
	o = solver->createOrder(type, set->clone(solver, map));
	map->put(this, o);
	return o;
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
