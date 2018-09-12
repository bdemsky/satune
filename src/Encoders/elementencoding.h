#ifndef ELEMENTENCODING_H
#define ELEMENTENCODING_H
#include "classlist.h"
#include "naiveencoder.h"
#include "constraint.h"

typedef enum ElemEnc {EENC_UNKNOWN, EENC_NONE, EENC_BOTH} ElemEnc;

class ElementEncoding {
public:
	ElementEncoding(Element *element);
	ElementEncodingType getElementEncodingType() {return type;}
	~ElementEncoding();
	void setElementEncodingType(ElementEncodingType type);
	void deleteElementEncoding();
	void allocEncodingArrayElement(uint size);
	void allocInUseArrayElement(uint size);
	uint numEncodingVars() {return numVars;}
	bool isinUseElement(uint offset) { return (inUseArray[(offset >> 6)] >> (offset & 63)) & 0x1;}
	void setInUseElement(uint offset) {inUseArray[(offset >> 6)] |= ((uint64_t)1) << (offset & 63);}
	void encodingArrayInitialization();
	uint getSizeEncodingArray(uint setSize) {
		switch (type) {
		case BINARYINDEX:
			return NEXTPOW2(setSize);
		case ONEHOT:
		case UNARY:
			return setSize;
		default:
			ASSERT(0);
		}
		return -1;
	}
	void print();

	ElementEncodingType type;
	Element *element;
	Edge *variables;/* List Variables Used To Encode Element */
	union {
		struct {
			uint64_t *encodingArray;	/* List the Variables in the appropriate order */
			uint64_t *inUseArray;	/* Bitmap to track variables in use */
			Edge *edgeArray;
			Polarity *polarityArray;
			uint encArraySize;
			ElemEnc encoding;
		};
		struct {
			uint64_t offset;/* Value = offset + encoded number (interpretted according to isBinaryValSigned) */
			uint64_t low;	/* Lowest value to encode */
			uint64_t high;/* High value to encode.   If low > high, we assume wrap around to include 0. */
			uint numBits;
			bool isBinaryValSigned;
		};
	};
	uint numVars;	/* Number of variables */
        CMEMALLOC;
};



#endif
