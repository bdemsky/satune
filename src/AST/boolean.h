#ifndef BOOLEAN_H
#define BOOLEAN_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"
#include "astnode.h"
#include "functionencoding.h"
#include "constraint.h"

/**
    This is a little sketchy, but apparently legit.
    https://www.python.org/dev/peps/pep-3123/ */

#define GETBOOLEANTYPE(o) (o->type)
#define GETBOOLEANPARENTS(o) (&(o->parents))
#define GETBOOLEANPOLARITY(b) (b->polarity)
#define GETBOOLEANVALUE(b) (b->boolVal)

class Boolean : public ASTNode {
public:
	Boolean(ASTNodeType _type);
	virtual ~Boolean() {}
	virtual Boolean *clone(CSolver *solver, CloneMap *map) { ASSERT(0); return NULL; }
	Polarity polarity;
	BooleanValue boolVal;
	Vector<Boolean *> parents;
	MEMALLOC;
};

class BooleanVar : public Boolean {
public:
	BooleanVar(VarType t);
	Boolean *clone(CSolver *solver, CloneMap *map);

	VarType vtype;
	Edge var;
	MEMALLOC;
};

class BooleanOrder : public Boolean {
public:
	BooleanOrder(Order *_order, uint64_t _first, uint64_t _second);
	Boolean *clone(CSolver *solver, CloneMap *map);

	Order *order;
	uint64_t first;
	uint64_t second;
	MEMALLOC;
};

class BooleanPredicate : public Boolean {
public:
	BooleanPredicate(Predicate *_predicate, Element **_inputs, uint _numInputs, Boolean *_undefinedStatus);
	Boolean *clone(CSolver *solver, CloneMap *map);

	Predicate *predicate;
	FunctionEncoding encoding;
	Array<Element *> inputs;
	Boolean *undefStatus;
	FunctionEncoding *getFunctionEncoding() {return &encoding;}
	MEMALLOC;
};

class BooleanLogic : public Boolean {
public:
	BooleanLogic(CSolver *solver, LogicOp _op, Boolean **array, uint asize);
	Boolean *clone(CSolver *solver, CloneMap *map);

	LogicOp op;
	Array<Boolean *> inputs;
	MEMALLOC;
};
#endif
