#ifndef ELEMENT_H
#define ELEMENT_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "astnode.h"
#include "functionencoding.h"
#include "elementencoding.h"
#include "boolean.h"

#define GETELEMENTTYPE(o) (o->type)
#define GETELEMENTPARENTS(o) (&((Element *)o)->parents)
class Element : public ASTNode {
 public:
	Element(ASTNodeType type);
	~Element();
	VectorASTNode parents;
	ElementEncoding encoding;
	MEMALLOC;
};

class ElementConst : public Element {
 public:
	ElementConst(uint64_t value, VarType type);
	~ElementConst();
	Set *set;
	uint64_t value;
	MEMALLOC;
};

class ElementSet : public Element {
 public:
	ElementSet(Set *s);
	Set *set;
	MEMALLOC;
};

class ElementFunction : public Element {
 public:
	ElementFunction(Function *function, Element **array, uint numArrays, Boolean *overflowstatus);
	~ElementFunction();
	Function *function;
	Array<Element *> inputs;
	Boolean *overflowstatus;
	FunctionEncoding functionencoding;
	MEMALLOC;
};

Set *getElementSet(Element *This);

static inline ElementEncoding *getElementEncoding(Element *e) {
	return &e->encoding;
}

static inline FunctionEncoding *getElementFunctionEncoding(ElementFunction *func) {
	return &func->functionencoding;
}
#endif
