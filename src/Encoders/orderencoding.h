#ifndef ORDERENCODING_H
#define ORDERENCODING_H
#include "classlist.h"

enum OrderEncodingType {
	ORDER_UNASSIGNED, PAIRWISE, INTEGERENCODING
};

typedef enum OrderEncodingType OrderEncodingType;

struct OrderEncoding {
	OrderEncodingType type;
	Order *order;
};

void initOrderEncoding(OrderEncoding *This, Order *order);
void deleteOrderEncoding(OrderEncoding *This);

#endif
