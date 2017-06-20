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
	uint numDomains;
	Set ** domains;
	Set * range;
	OverFlowBehavior overflowbehavior;
};

struct FunctionTable {
	Function base;
	Table* table;
};

Function* allocFunctionOperator( ArithOp op, Set ** domain, uint numDomain, Set * range,OverFlowBehavior overflowbehavior);
Function* allocFunctionTable (Table* table);
void deleteFunction(Function* This);

#endif
