#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"
#include "ordergraph.h"
#include "csolver.h"
#include "orderpairresolver.h"

Order::Order(OrderType _type, Set *_set) :
	type(_type),
	set(_set),
	graph(NULL),
	encoding(this)
{
}

void Order::addOrderConstraint(BooleanOrder *constraint) {
	constraints.push(constraint);
	useditems.add(constraint->first);
	useditems.add(constraint->second);
}

void Order::setOrderEncodingType(OrderEncodingType type) {
	encoding.type = type;
}

Order *Order::clone(CSolver *solver, CloneMap *map) {
	Order *o = (Order *)map->get(this);
	if (o != NULL)
		return o;
	o = solver->createOrder(type, set->clone(solver, map));
	map->put(this, o);
	return o;
}

HashtableOrderPair *Order::getOrderPairTable() {
	ASSERT( encoding.resolver != NULL);
	if (OrderPairResolver *t = dynamic_cast<OrderPairResolver *>(encoding.resolver)) {
		return t->getOrderPairTable();
	} else {
		ASSERT(0);
	}
}

Order::~Order() {
	if (graph != NULL) {
		delete graph;
	}
}

void Order::serialize(Serializer *serializer) {
	if (serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	set->serialize(serializer);
	ASTNodeType asttype = ORDERTYPE;
	serializer->mywrite(&asttype, sizeof(ASTNodeType));
	Order *This = this;
	serializer->mywrite(&This, sizeof(Order *));
	serializer->mywrite(&type, sizeof(OrderType));
	serializer->mywrite(&set, sizeof(Set *));
}

void Order::print() {
	model_print("{Order<%p> on Set:\n", this);
	set->print();
	model_print("}\n");
}
