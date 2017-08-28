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
	virtual ~Element() {}
	Vector<ASTNode *> parents;
	ElementEncoding encoding;
	virtual Element *clone(CSolver *solver, CloneMap *map) {ASSERT(0); return NULL;};
	MEMALLOC;
};

class ElementConst : public Element {
public:
	ElementConst(uint64_t value, VarType type, Set *_set);
	Set *set;
	uint64_t value;
	Element *clone(CSolver *solver, CloneMap *map);
	MEMALLOC;
};

class ElementSet : public Element {
public:
	ElementSet(Set *s);
	Set *set;
	Element *clone(CSolver *solver, CloneMap *map);
	MEMALLOC;
};

class ElementFunction : public Element {
public:
	ElementFunction(Function *function, Element **array, uint numArrays, Boolean *overflowstatus);
	Function *function;
	Array<Element *> inputs;
	Boolean *overflowstatus;
	FunctionEncoding functionencoding;
	Element *clone(CSolver *solver, CloneMap *map);
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
