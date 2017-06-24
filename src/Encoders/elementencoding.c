#include "elementencoding.h"

void initElementEncoding(ElementEncoding * This, Element *element) {
	This->element=element;
	This->type=ELEM_UNASSIGNED;
	This->variables=NULL;
	This->encodingArray=NULL;
	This->inUseArray=NULL;
	This->numVars=0;
}

void deleteElementEncoding(ElementEncoding *This) {
	if (This->variables!=NULL)
		ourfree(This->variables);
	if (This->encodingArray!=NULL)
		ourfree(This->encodingArray);
	if (This->inUseArray!=NULL)
		ourfree(This->inUseArray);
}

void allocEncodingArrayElement(ElementEncoding *This, uint size) {
	This->encodingArray=ourcalloc(1, sizeof(uint64_t)*size);
}

void allocInUseArrayElement(ElementEncoding *This, uint size) {
	This->inUseArray=ourcalloc(1, size >> 6);
}

void setElementEncodingType(ElementEncoding* This, ElementEncodingType type){
	This->type = type;
}
