#include "elementencoder.h"

ElementEncoder * allocElementEncoder(ElementEncoderType type, Element *element) {
	ElementEncoder * this=(ElementEncoder *)ourmalloc(sizeof(ElementEncoder));
	this->element=element;
	this->type=type;
	return this;
}

void deleteElementEncoder(ElementEncoder *this) {
	ourfree(this);
}
