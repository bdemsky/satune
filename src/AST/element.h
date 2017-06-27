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
#define GETELEMENTENCODING(e) (GETELEMENTTYPE(e)==ELEMSET?	\
		&((ElementSet*)e)->encoding:	\
		GETELEMENTTYPE(e)==ELEMFUNCRETURN?	\
		&((ElementFunction*)e)->domainencoding: NULL)
// Should be called on the element or boolean
#define GETFUNCTIONENCODING(f) (GETASTNODETYPE(f) == ELEMFUNCRETURN?	\
		&((ElementFunction*)f)->functionencoding:	\
		GETASTNODETYPE(f) == PREDICATEOP?	\
		&((BooleanPredicate*)f)->encoding: NULL)
		
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

uint getElementSize(Element* This);
Constraint * getElementValueConstraint(Element* This, uint64_t value);
#endif
