#ifndef ELEMENTENCODER_H
#define ELEMENTENCODER_H
#include "classlist.h"

enum ElementEncoderType {
	ONEHOT, UNARY, BINARYINDEX, ONEHOTBINARY, BINARYVAL
};

typedef enum ElementEncoderType ElementEncoderType;

struct ElementEncoder {
	ElementEncoderType type;
	Element * element;
};

ElementEncoder * allocElementEncoder(ElementEncoderType type, Element *element);
void deleteElementEncoder(ElementEncoder *this);
#endif
