#ifndef ELEMENT_H
#define ELEMENT_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"

#define GETELEMENTTYPE(o) (((Element*)o)->type)

//FIXME:TALK ABOUT ELEMENT
struct Element {
	ElementType type;
};

struct ElementSet {
	Element base;
	Set * set;
	ElementEncoding * encoding;
};

struct ElementFunction {
	Element base;
	Function * function;
	VectorElement* Elements;
	Boolean * overflowstatus;
};

Element * allocElementSet(Set *s);
Element* allocElementFunction(Function * function, Element ** array, uint numArrays, Boolean * overflowstatus);
void deleteElement(Element *This);
#endif
