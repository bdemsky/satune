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
	Boolean ** variables; /* List Variables Use To Encode Element */
	uint64_t * encodingArray; /* List the Variables in the appropriate order */
	uint numVars; /* Number of variables */
};

ElementEncoding * allocElementEncoding(ElementEncodingType type, Element *element);
void deleteElementEncoding(ElementEncoding *This);
#endif
