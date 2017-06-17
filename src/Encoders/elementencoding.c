#include "elementencoding.h"

ElementEncoding * allocElementEncoding(ElementEncodingType type, Element *element) {
	ElementEncoding * this=(ElementEncoding *)ourmalloc(sizeof(ElementEncoding));
	this->element=element;
	this->type=type;
	this->variables=NULL;
	this->encodingArray=NULL;
	this->numVars=0;
	return this;
}

void deleteElementEncoding(ElementEncoding *this) {
	if (this->variables!=NULL)
		ourfree(this->variables);
	if (this->encodingArray!=NULL)
		ourfree(this->encodingArray);
	ourfree(this);
}
