#ifndef FUNCTION_H
#define FUNCTION_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "astops.h"
#include "structs.h"

class Function {
public:
	Function(FunctionType _type) : type(_type) {}
	FunctionType type;
	virtual ~Function() {}
	virtual Function *clone(CSolver *solver, CloneMap *map) {ASSERT(0); return NULL;}
	virtual void serialize(Serializer *serialiezr) = 0;
	virtual void print() = 0;
	virtual Set *getRange() = 0;
	CMEMALLOC;
};

class FunctionOperator : public Function {
public:
	ArithOp op;
	Set *range;
	OverFlowBehavior overflowbehavior;
	FunctionOperator(ArithOp op, Set *range, OverFlowBehavior overflowbehavior);
	uint64_t applyFunctionOperator(uint numVals, uint64_t *values);
	bool isInRangeFunction(uint64_t val);
	Function *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer *serialiezr);
	virtual void print();
	Set *getRange() {return range;}
	CMEMALLOC;
};

class FunctionTable : public Function {
public:
	Table *table;
	UndefinedBehavior undefBehavior;
	FunctionTable (Table *table, UndefinedBehavior behavior);
	Function *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer *serialiezr);
	virtual void print();
	Set *getRange();
	CMEMALLOC;
};

#endif
