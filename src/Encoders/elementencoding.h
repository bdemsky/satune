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
	Constraint ** variables; /* List Variables Used To Encode Element */
	uint64_t * encodingArray; /* List the Variables in the appropriate order */
	uint64_t * inUseArray; /* Bitmap to track variables in use */
	uint numVars; /* Number of variables */
};

ElementEncoding * allocElementEncoding(ElementEncodingType type, Element *element);
void deleteElementEncoding(ElementEncoding *This);
void baseBinaryIndexElementAssign(ElementEncoding *This);
void allocEncodingArrayElement(ElementEncoding *This, uint size);
void allocInUseArrayElement(ElementEncoding *This, uint size);

inline bool isinUseElement(ElementEncoding *This, uint offset) {
	return (This->inUseArray[(offset>>6)] >> (offset & 63)) &0x1;
}

inline void setInUseElement(ElementEncoding *This, uint offset) {
	This->inUseArray[(offset>>6)] |= 1 << (offset & 63);
}
#endif
