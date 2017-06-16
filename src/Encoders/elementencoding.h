#ifndef ELEMENTENCODING_H
#define ELEMENTENCODING_H
#include "classlist.h"

enum ElementEncodingType {
	ONEHOT, UNARY, BINARYINDEX, ONEHOTBINARY, BINARYVAL
};

typedef enum ElementEncodingType ElementEncodingType;

struct ElementEncoding {
	ElementEncodingType type;
	Element * element;
};

ElementEncoding * allocElementEncoding(ElementEncodingType type, Element *element);
void deleteElementEncoding(ElementEncoding *this);
#endif
