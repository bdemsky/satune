#ifndef BOOLEAN_H
#define BOOLEAN_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"
#include "structtype.h"
/**
    This is a little sketchy, but apparently legit.
    https://www.python.org/dev/peps/pep-3123/ */

#define GETBOOLEANTYPE(o) (((Boolean *)(o))->btype)

struct Boolean {
	Struct stype;
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
	Constraint * var;
};

struct BooleanLogic {
	Boolean base;
	LogicOp op;
	Boolean * left;
	Boolean * right;
};

struct BooleanPredicate {
	Boolean base;
	Predicate * predicate;
	VectorElement* inputs;
};

Boolean * allocBoolean(VarType t);
Boolean * allocBooleanOrder(Order * order, uint64_t first, uint64_t second);
Boolean * allocBooleanPredicate(Predicate * predicate, Element ** inputs, uint numInputs);
Boolean * allocBooleanLogic(LogicOp op, Boolean * left, Boolean* right);
/**
 * This function also save new boooleans to solver->allbooleans
 * @param solver
 * @param op
 * @param array
 * @param asize
 * @return
 */
Boolean * allocBooleanLogicArray(CSolver *solver, LogicOp op, Boolean ** array, uint asize);
void deleteBoolean(Boolean * This);

#endif
