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
	return order;
}

void deleteOrder(Order* order){
	deleteVectorArrayBoolean(& order->constraints);
	deleteOrderEncoding(& order->order);
	ourfree(order);
}
