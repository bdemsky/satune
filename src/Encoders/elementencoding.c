#include "elementencoding.h"

ElementEncoding * allocElementEncoding(ElementEncodingType type, Element *element) {
	ElementEncoding * This=(ElementEncoding *)ourmalloc(sizeof(ElementEncoding));
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
	ourfree(This);
}
