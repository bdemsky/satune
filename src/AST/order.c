#include "order.h"
#include "structs.h"
#include "set.h"
#include "boolean.h"


Order* allocOrder(OrderType type, Set * set){
	Order* order = (Order*)ourmalloc(sizeof(Order));
	order->set=set;
	order->constraints = allocDefVectorBoolean();
	order->type=type;
	return order;
}

void deleteOrder(Order* order){
	uint size = getSizeVectorBoolean( order->constraints );
	for(uint i=0; i<size; i++){
		deleteBoolean( getVectorBoolean(order->constraints, i) );
	}
	deleteSet( order->set);
	ourfree(order);
}
