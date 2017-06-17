#ifndef BOOLEAN_H
#define BOOLEAN_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"

struct BooleanOrder {
	Order* order;
	uint64_t first;
	uint64_t second;
};

struct BooleanVar {
	VarType vtype;
};

struct BooleanLogic {
	LogicOp op;
	Boolean * left;
	Boolean * right;
};

struct BooleanComp {
	CompOp op;
	Boolean * left;
	Boolean * right;
};

struct Boolean {
	BooleanType btype;
	union {
		BooleanOrder order;
		BooleanVar var;
		BooleanLogic logic;
		BooleanComp comp;
	};
};

Boolean * allocBoolean(VarType t);
Boolean * allocBooleanOrder(Order * order, uint64_t first, uint64_t second);
void deleteBoolean(Boolean * this);

#endif
