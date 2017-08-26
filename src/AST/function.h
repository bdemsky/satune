#ifndef FUNCTION_H
#define FUNCTION_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"

#define GETFUNCTIONTYPE(o) (((Function *)o)->type)

class Function {
public:
	Function(FunctionType _type) : type(_type) {}
	FunctionType type;
	MEMALLOC;
	virtual ~Function() {}
};

class FunctionOperator : public Function {
public:
	ArithOp op;
	Array<Set *> domains;
	Set *range;
	OverFlowBehavior overflowbehavior;
	FunctionOperator(ArithOp op, Set **domain, uint numDomain, Set *range, OverFlowBehavior overflowbehavior);
	uint64_t applyFunctionOperator(uint numVals, uint64_t *values);
	bool isInRangeFunction(uint64_t val);
	MEMALLOC;
};

class FunctionTable : public Function {
public:
	Table *table;
	UndefinedBehavior undefBehavior;
	FunctionTable (Table *table, UndefinedBehavior behavior);
	MEMALLOC;
};

#endif
