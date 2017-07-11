#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"

Order* allocOrder(OrderType type, Set * set){
	Order* This = (Order*)ourmalloc(sizeof(Order));
	This->set=set;
	initDefVectorBoolean(& This->constraints);
	This->type=type;
	initOrderEncoding(& This->order, This);
	This->boolsToConstraints = NULL;
	return This;
}

void initializeOrderHashTable(Order* This){
	This->boolsToConstraints= allocHashTableBoolConst(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
}

void addOrderConstraint(Order* This, BooleanOrder* constraint){
	pushVectorBoolean( &This->constraints, (Boolean*) constraint);
}

void setOrderEncodingType(Order* This, OrderEncodingType type){
	This->order.type = type;
}

void deleteOrder(Order* This){
	deleteVectorArrayBoolean(& This->constraints);
	deleteOrderEncoding(& This->order);
	if(This->boolsToConstraints!= NULL) {
		resetAndDeleteHashTableBoolConst(This->boolsToConstraints);
		deleteHashTableBoolConst(This->boolsToConstraints);
	}
	ourfree(This);
}
