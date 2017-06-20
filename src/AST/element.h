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
	Set * set;
	ElementEncoding * encoding;
};

struct ElementSet {
    Element base;
    Set * set;
};

struct ElementFunction{
    Element base;
    Function * function;
    VectorElement* Elements;
    Boolean * overflowstatus;
};

Element * allocElement(Set *s);
Element* allocElementFunction(Function * function, Element ** array, uint numArrays, Boolean * overflowstatus);
void deleteElement(Element *This);
#endif
