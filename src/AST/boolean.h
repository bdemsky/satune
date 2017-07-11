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

#define GETBOOLEANTYPE(o) GETASTNODETYPE(o)
#define GETBOOLEANPARENTS(o) (&((Boolean *)(o))->parents)

struct Boolean {
	ASTNode base;
	VectorBoolean parents;
};

struct BooleanOrder {
	Boolean base;
	Order* order;
	uint64_t first;
	uint64_t second;
};

struct BooleanVar {
	Boolean base;
	VarType vtype;
	Edge var;
};

struct BooleanLogic {
	Boolean base;
	LogicOp op;
	ArrayBoolean inputs;
};

struct BooleanPredicate {
	Boolean base;
	Predicate * predicate;
	FunctionEncoding encoding;
	ArrayElement inputs;
};

Boolean * allocBoolean(VarType t);
Boolean * allocBooleanOrder(Order * order, uint64_t first, uint64_t second);
Boolean * allocBooleanPredicate(Predicate * predicate, Element ** inputs, uint numInputs);
Boolean * allocBooleanLogicArray(CSolver *solver, LogicOp op, Boolean ** array, uint asize);
void deleteBoolean(Boolean * This);

static inline FunctionEncoding* getPredicateFunctionEncoding(BooleanPredicate* func){
	return &func->encoding;
}

#endif
