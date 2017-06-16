#include "elementencoding.h"

ElementEncoding * allocElementEncoding(ElementEncodingType type, Element *element) {
	ElementEncoding * this=(ElementEncoding *)ourmalloc(sizeof(ElementEncoding));
	this->element=element;
	this->type=type;
	return this;
}

void deleteElementEncoding(ElementEncoding *this) {
	ourfree(this);
}
