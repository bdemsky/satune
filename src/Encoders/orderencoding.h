#ifndef ORDERENCODING_H
#define ORDERENCODING_H
#include "classlist.h"

enum OrderEncodingType {
	ORDER_UNASSIGNED, PAIRWISE, INTEGERENCODING
};

typedef enum OrderEncodingType OrderEncodingType;

class OrderEncoding {
public:
	OrderEncoding(Order *order);

	OrderEncodingType type;
	Order *order;
	CMEMALLOC;
};

#endif
