#ifndef ELEMENT_H
#define ELEMENT_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "astnode.h"
#include "functionencoding.h"
#include "elementencoding.h"
#include "boolean.h"

class Element : public ASTNode {
public:
	Element(ASTNodeType type);
	virtual ~Element() {}
	Vector<ASTNode *> parents;
	ElementEncoding encoding;
	virtual Element *clone(CSolver *solver, CloneMap *map) {ASSERT(0); return NULL;};
	virtual void serialize(Serializer* serializer) =0;
	virtual void updateParents() {}
	virtual Set * getRange() = 0;
	CMEMALLOC;
};

class ElementSet : public Element {
public:
	ElementSet(ASTNodeType type, Set *s);
	ElementSet(Set *s);
	virtual Element *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer* serializer);
	CMEMALLOC;
	Set *getRange() {return set;}
 protected:
	Set *set;

};

class ElementConst : public ElementSet {
public:
	ElementConst(uint64_t value, Set *_set);
	uint64_t value;
	virtual void serialize(Serializer* serializer);
	Element *clone(CSolver *solver, CloneMap *map);
	CMEMALLOC;
};

class ElementFunction : public Element {
public:
	ElementFunction(Function *function, Element **array, uint numArrays, BooleanEdge overflowstatus);
	Function *function;
	Array<Element *> inputs;
	BooleanEdge overflowstatus;
	FunctionEncoding functionencoding;
	Element *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer* serializer);
	Set * getRange();
	void updateParents();
	CMEMALLOC;
};

static inline ElementEncoding *getElementEncoding(Element *e) {
	return &e->encoding;
}

static inline FunctionEncoding *getElementFunctionEncoding(ElementFunction *func) {
	return &func->functionencoding;
}
#endif
