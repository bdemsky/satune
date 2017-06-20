#include "elementencoding.h"

ElementEncoding * allocElementEncoding(ElementEncodingType type, Element *element) {
	ElementEncoding * This=ourmalloc(sizeof(ElementEncoding));
	This->element=element;
	This->type=type;
	This->variables=NULL;
	This->encodingArray=NULL;
	This->numVars=0;
	return This;
}

void deleteElementEncoding(ElementEncoding *This) {
	if (This->variables!=NULL)
		ourfree(This->variables);
	if (This->encodingArray!=NULL)
		ourfree(This->encodingArray);
	if (This->inUseArray!=NULL)
		ourfree(This->inUseArray);
	ourfree(This);
}

void allocEncodingArrayElement(ElementEncoding *This, uint size) {
	This->encodingArray=ourcalloc(1, sizeof(uint64_t)*size);
}

void allocInUseArrayElement(ElementEncoding *This, uint size) {
	This->inUseArray=ourcalloc(1, size >> 6);
}
