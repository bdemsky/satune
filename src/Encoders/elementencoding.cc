#include "elementencoding.h"
#include "common.h"
#include "naiveencoder.h"
#include "element.h"
#include "satencoder.h"
#include "set.h"

ElementEncoding::ElementEncoding(Element *_element) :
	type(ELEM_UNASSIGNED),
	element(_element),
	variables(NULL),
	encodingArray(NULL),
	inUseArray(NULL),
	encArraySize(0),
	numVars(0) {
}

ElementEncoding::~ElementEncoding() {
	if (variables != NULL)
		ourfree(variables);
	if (encodingArray != NULL)
		ourfree(encodingArray);
	if (inUseArray != NULL)
		ourfree(inUseArray);
}

void ElementEncoding::allocEncodingArrayElement(uint size) {
	encodingArray = (uint64_t *) ourcalloc(1, sizeof(uint64_t) * size);
	encArraySize = size;
}

void ElementEncoding::allocInUseArrayElement(uint size) {
	uint bytes = ((size + ((1 << 9) - 1)) >> 6) & ~7;//Depends on size of inUseArray
	inUseArray = (uint64_t *) ourcalloc(1, bytes);
}

void ElementEncoding::setElementEncodingType(ElementEncodingType _type) {
	type = _type;
}

void ElementEncoding::encodingArrayInitialization() {
	Set *set = getElementSet(element);
	uint size = set->getSize();
	uint encSize = getSizeEncodingArray(size);
	allocEncodingArrayElement(encSize);
	allocInUseArrayElement(encSize);
	for (uint i = 0; i < size; i++) {
		encodingArray[i] = set->getMemberAt(i);
		setInUseElement(i);
	}
}
