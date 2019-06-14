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
	inline ElementEncoding *getElementEncoding() { return &encoding; }
	inline void freezeElement(){frozen = true;}
	inline bool isFrozen(){return frozen;}
	virtual Element *clone(CSolver *solver, CloneMap *map) {ASSERT(0); return NULL;};
	virtual void serialize(Serializer *serializer) = 0;
	virtual void print() = 0;
	virtual void updateParents() {}
	virtual Set *getRange() = 0;
	CMEMALLOC;
	bool anyValue;
	bool frozen;
};

class ElementSet : public Element {
public:
	ElementSet(ASTNodeType type, Set *s);
	virtual ~ElementSet() {}
	ElementSet(Set *s);
	virtual Element *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer *serializer);
	virtual void print();
	CMEMALLOC;
	Set *getRange() {return set;}
protected:
	Set *set;
	friend class ElementOpt;
};

class ElementConst : public ElementSet {
public:
	ElementConst(uint64_t value, Set *_set);
	virtual ~ElementConst() {}
	uint64_t value;
	virtual void serialize(Serializer *serializer);
	virtual void print();
	Element *clone(CSolver *solver, CloneMap *map);
	CMEMALLOC;
};

class ElementFunction : public Element {
public:
	virtual ~ElementFunction() {}
	ElementFunction(Function *function, Element **array, uint numArrays, BooleanEdge overflowstatus);
	Array<Element *> inputs;
	BooleanEdge overflowstatus;
	FunctionEncoding functionencoding;
	Element *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer *serializer);
	virtual void print();
	Set *getRange();
	void updateParents();
	Function *getFunction() {return function;}
	inline FunctionEncoding *getElementFunctionEncoding() {return &functionencoding;}
	CMEMALLOC;
private:
	Function *function;
};


#endif
