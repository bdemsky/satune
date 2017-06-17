#ifndef FUNCTION_H
#define FUNCTION_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"
struct Function {
	ArithOp op;
	VectorSet* domains;
	Set * range;
	OverFlowBehavior overflowbehavior;
	Table* table;
};
#endif
