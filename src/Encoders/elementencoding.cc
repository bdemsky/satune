#include "elementencoding.h"
#include "common.h"
#include "naiveencoder.h"
#include "element.h"
#include "satencoder.h"
#include "set.h"

const char *elemEncTypeNames[] = {"UNASSIGNED", "ONEHOT", "UNARY", "BINARYINDEX", "BINARYVAL"};

ElementEncoding::ElementEncoding(Element *_element) :
	type(ELEM_UNASSIGNED),
	element(_element),
	variables(NULL),
	encodingArray(NULL),
	inUseArray(NULL),
	edgeArray(NULL),
	polarityArray(NULL),
	encArraySize(0),
	encoding(EENC_UNKNOWN),
	numVars(0) {
}

ElementEncoding::~ElementEncoding() {
	if (variables != NULL)
		ourfree(variables);
	if (encodingArray != NULL)
		ourfree(encodingArray);
	if (inUseArray != NULL)
		ourfree(inUseArray);
	if (edgeArray != NULL)
		ourfree(edgeArray);
	if (polarityArray != NULL)
		ourfree(polarityArray);
}

void ElementEncoding::allocEncodingArrayElement(uint size) {
	encodingArray = (uint64_t *) ourcalloc(1, sizeof(uint64_t) * size);
	encArraySize = size;
}

void ElementEncoding::allocInUseArrayElement(uint size) {
	uint bytes = ((size + 63) >> 3) & ~7;	//Depends on size of inUseArray
	inUseArray = (uint64_t *) ourcalloc(1, bytes);
}

void ElementEncoding::setElementEncodingType(ElementEncodingType _type) {
	type = _type;
}

void ElementEncoding::encodingArrayInitialization() {
	Set *set = element->getRange();
	uint size = set->getSize();
	uint encSize = getSizeEncodingArray(size);
	allocEncodingArrayElement(encSize);
	allocInUseArrayElement(encSize);
	for (uint i = 0; i < size; i++) {
		encodingArray[i] = set->getElement(i);
		setInUseElement(i);
	}
}

void ElementEncoding::print() {
	model_print("%s ", elemEncTypeNames[type]);
	if (type == BINARYINDEX) {
		for (uint i = 0; i < encArraySize; i++) {
			if (i != 0)
				model_print(", ");
			if (isinUseElement(i))
				model_print("%" PRIu64 "", encodingArray[i]);
			else
				model_print("_");
		}
	}
}
