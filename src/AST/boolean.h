#ifndef BOOLEAN_H
#define BOOLEAN_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"

/** 
		This is a little sketchy, but apparently legit.
		https://www.python.org/dev/peps/pep-3123/ */

struct Boolean {
	BooleanType btype;
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
};

struct BooleanLogic {
	Boolean base;
	LogicOp op;
	Boolean * left;
	Boolean * right;
};

struct BooleanComp {
	Boolean base;
	CompOp op;
	Boolean * left;
	Boolean * right;
};



Boolean * allocBoolean(VarType t);
Boolean * allocBooleanOrder(Order * order, uint64_t first, uint64_t second);
void deleteBoolean(Boolean * this);

#endif
