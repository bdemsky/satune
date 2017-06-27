#include "elementencoding.h"
#include "common.h"
#include "naiveencoder.h"
#include "element.h"

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

void allocElementConstraintVariables(ElementEncoding* This, uint numVars){
	This->numVars = numVars;
	This->variables = ourmalloc(sizeof(Constraint*) * numVars);
}

void setElementEncodingType(ElementEncoding* This, ElementEncodingType type){
	This->type = type;
}

void generateBinaryIndexEncodingVars(NaiveEncoder* encoder, ElementEncoding* This){
	ASSERT(This->type==BINARYINDEX);
	uint size = getElementSize(This->element);
	allocElementConstraintVariables(This, NUMBITS(size-1));
	getArrayNewVars(encoder, This->numVars, This->variables);
}

void generateElementEncodingVariables(NaiveEncoder* encoder, ElementEncoding* This){
	ASSERT(This->type!=ELEM_UNASSIGNED);
	ASSERT(This->variables==NULL);
	switch(This->type){
		case BINARYINDEX:
			generateBinaryIndexEncodingVars(encoder, This);
			break;
		default:
			ASSERT(0);
	}
}