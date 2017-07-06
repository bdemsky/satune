#ifndef FUNCTION_H
#define FUNCTION_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"

#define GETFUNCTIONTYPE(o) (((Function*)o)->type)

struct Function {
	FunctionType type;
};

struct FunctionOperator {
	Function base;
	ArithOp op;
	ArraySet domains;
	Set * range;
	OverFlowBehavior overflowbehavior;
};

struct FunctionTable {
	Function base;
	Table* table;
};

Function* allocFunctionOperator( ArithOp op, Set ** domain, uint numDomain, Set * range,OverFlowBehavior overflowbehavior);
Function* allocFunctionTable (Table* table);
uint64_t applyFunctionOperator(FunctionOperator* func, uint64_t var1, uint64_t var2, bool* isInrange, bool* hasOverFlow);
void deleteFunction(Function* This);

#endif
