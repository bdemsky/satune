#ifndef ORDERENCODING_H
#define ORDERENCODING_H
#include "classlist.h"

enum OrderEncodingType {
	PAIRWISE
};

typedef enum OrderEncodingType OrderEncodingType;

struct OrderEncoding {
	OrderEncodingType type;
	Order *order;
};

OrderEncoding * allocOrderEncoding(OrderEncodingType type, Order *order);
void deleteOrderEncoding(OrderEncoding *This);

#endif
