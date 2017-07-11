#include "elementencoding.h"
#include "common.h"
#include "naiveencoder.h"
#include "element.h"
#include "satencoder.h"

void initElementEncoding(ElementEncoding * This, Element *element) {
	This->element=element;
	This->type=ELEM_UNASSIGNED;
	This->variables=NULL;
	This->encodingArray=NULL;
	This->inUseArray=NULL;
	This->numVars=0;
	This->encArraySize=0;
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
	This->encArraySize=size;
}

void allocInUseArrayElement(ElementEncoding *This, uint size) {
	uint bytes = ((size + ((1 << 9)-1)) >> 6)&~7;//Depends on size of inUseArray
	This->inUseArray=ourcalloc(1, bytes);
}

void allocElementConstraintVariables(ElementEncoding* This, uint numVars){
	This->numVars = numVars;
	This->variables = ourmalloc(sizeof(Edge) * numVars);
}

void setElementEncodingType(ElementEncoding* This, ElementEncodingType type){
	This->type = type;
}

void generateBinaryIndexEncodingVars(SATEncoder* encoder, ElementEncoding* This){
	ASSERT(This->type==BINARYINDEX);
	allocElementConstraintVariables(This, NUMBITS(This->encArraySize-1));
	getArrayNewVarsSATEncoder(encoder, This->numVars, This->variables);
}

void generateElementEncodingVariables(SATEncoder* encoder, ElementEncoding* This){
	ASSERT(This->type!=ELEM_UNASSIGNED);
	if(This->variables!=NULL)
		return;
	switch(This->type){
		case BINARYINDEX:
			generateBinaryIndexEncodingVars(encoder, This);
			break;
		default:
			ASSERT(0);
	}
}
