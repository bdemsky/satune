#ifndef ELEMENTENCODING_H
#define ELEMENTENCODING_H
#include "classlist.h"
#include "naiveencoder.h"
#include "constraint.h"

enum ElementEncodingType {
	ELEM_UNASSIGNED, ONEHOT, UNARY, BINARYINDEX, ONEHOTBINARY, BINARYVAL
};

typedef enum ElementEncodingType ElementEncodingType;

struct ElementEncoding {
	ElementEncodingType type;
	Element * element;
	Edge * variables;/* List Variables Used To Encode Element */
	uint64_t * encodingArray;	/* List the Variables in the appropriate order */
	uint64_t * inUseArray;/* Bitmap to track variables in use */
	uint encArraySize;
	uint numVars;	/* Number of variables */
};

void initElementEncoding(ElementEncoding *This, Element *element);
static inline ElementEncodingType getElementEncodingType(ElementEncoding * This) {return This->type;}
void setElementEncodingType(ElementEncoding* This, ElementEncodingType type);
void deleteElementEncoding(ElementEncoding *This);
void allocEncodingArrayElement(ElementEncoding *This, uint size);
void allocInUseArrayElement(ElementEncoding *This, uint size);
void generateBinaryIndexEncodingVars(SATEncoder* encode, ElementEncoding* This);
void generateElementEncodingVariables(SATEncoder* encoder, ElementEncoding* This);
static inline uint numEncodingVars(ElementEncoding *This) {return This->numVars;}

static inline bool isinUseElement(ElementEncoding *This, uint offset) {
	return (This->inUseArray[(offset>>6)] >> (offset & 63)) &0x1;
}

static inline void setInUseElement(ElementEncoding *This, uint offset) {
	This->inUseArray[(offset>>6)] |= 1 << (offset & 63);
}

#endif
