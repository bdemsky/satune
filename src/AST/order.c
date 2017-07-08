#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"


Order* allocOrder(OrderType type, Set * set){
	Order* order = (Order*)ourmalloc(sizeof(Order));
	order->set=set;
	allocInlineDefVectorBoolean(& order->constraints);
	order->type=type;
	allocOrderEncoding(& order->order, order);
	order->boolsToConstraints = NULL;
	return order;
}

void initializeOrderHashTable(Order* order){
	order->boolsToConstraints= allocHashTableBoolConst(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
}

void addOrderConstraint(Order* order, BooleanOrder* constraint){
	pushVectorBoolean( &order->constraints, (Boolean*) constraint);
}

void setOrderEncodingType(Order* order, OrderEncodingType type){
	order->order.type = type;
}

void deleteOrder(Order* order){
	deleteVectorArrayBoolean(& order->constraints);
	deleteOrderEncoding(& order->order);
	deleteHashTableBoolConst(order->boolsToConstraints);
	ourfree(order);
}
