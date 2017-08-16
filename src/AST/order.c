#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"

Order* allocOrder(OrderType type, Set * set){
	Order* This = (Order*)ourmalloc(sizeof(Order));
	This->set=set;
	initDefVectorBooleanOrder(& This->constraints);
	This->type=type;
	initOrderEncoding(& This->order, This);
	This->orderPairTable = NULL;
	return This;
}

void initializeOrderHashTable(Order* This){
	This->orderPairTable=allocHashTableOrderPair(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
}

void addOrderConstraint(Order* This, BooleanOrder* constraint){
	pushVectorBooleanOrder( &This->constraints, constraint);
}

void setOrderEncodingType(Order* This, OrderEncodingType type){
	This->order.type = type;
}

void deleteOrder(Order* This){
	deleteVectorArrayBooleanOrder(& This->constraints);
	deleteOrderEncoding(& This->order);
	if(This->orderPairTable != NULL) {
		resetAndDeleteHashTableOrderPair(This->orderPairTable);
		deleteHashTableOrderPair(This->orderPairTable);
	}
	ourfree(This);
}
