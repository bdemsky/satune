#ifndef ELEMENT_H
#define ELEMENT_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "astnode.h"
#include "functionencoding.h"
#include "elementencoding.h"

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
#endif
