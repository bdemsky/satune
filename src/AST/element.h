#ifndef ELEMENT_H
#define ELEMENT_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "astnode.h"
#include "functionencoding.h"
#include "elementencoding.h"
#include "boolean.h"

#define GETELEMENTTYPE(o) GETASTNODETYPE(o)
#define GETELEMENTPARENTS(o) (&((Element*)o)->parents)
		
struct Element {
	ASTNode base;
	VectorASTNode parents;
};

struct ElementSet {
	Element base;
	Set * set;
	ElementEncoding encoding;
};

struct ElementFunction {
	Element base;
	Function * function;
	ArrayElement inputs;
	Boolean * overflowstatus;
	FunctionEncoding functionencoding;
	ElementEncoding domainencoding;
};

Element * allocElementSet(Set *s);
Element* allocElementFunction(Function * function, Element ** array, uint numArrays, Boolean * overflowstatus);
void deleteElement(Element *This);

static inline ElementEncoding* getElementEncoding(Element* This){
	switch(GETELEMENTTYPE(This)){
		case ELEMSET:
			return &((ElementSet*)This)->encoding;
		case ELEMFUNCRETURN:		
			return &((ElementFunction*)This)->domainencoding;
		default:
			ASSERT(0);
	}
	return NULL;
}


static inline FunctionEncoding* getElementFunctionEncoding(ElementFunction* func){
	return &func->functionencoding;
}

uint getElementSize(Element* This);
Constraint * getElementValueConstraint(Element* This, uint64_t value);
#endif
