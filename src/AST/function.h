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
	UndefinedBehavior undefBehavior;
};

Function* allocFunctionOperator(ArithOp op, Set ** domain, uint numDomain, Set * range, OverFlowBehavior overflowbehavior);
Function* allocFunctionTable (Table* table, UndefinedBehavior behavior);
uint64_t applyFunctionOperator(FunctionOperator* This, uint numVals, uint64_t * values);
bool isInRangeFunction(FunctionOperator *This, uint64_t val);
void deleteFunction(Function* This);

#endif
