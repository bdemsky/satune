#ifndef CSOLVER_H
#define CSOLVER_H
#include "classlist.h"
#include "ops.h"
#include "structs.h"

struct CSolver {
	VectorBoolean * constraints;
	VectorSet * sets;
	VectorElement * elements;
};

CSolver * allocCSolver();
Set * createSet(CSolver *, VarType type, uint64_t * elements, uint num);
Set * createRangeSet(CSolver *, VarType type, uint64_t lowrange, uint64_t highrange);
MutableSet * createMutableSet(CSolver *, VarType type);

void addItem(CSolver *, MutableSet * set, uint64_t element);
uint64_t createUniqueItem(CSolver *, MutableSet * set);

Element * getElementVar(CSolver *, Set * set);
Boolean * getBooleanVar(CSolver *, VarType type);

Function * createFunctionOperator(CSolver *solver, enum ArithOp op, Set ** domain, uint numDomain, Set * range,
        enum OverFlowBehavior overflowbehavior, Boolean * overflowstatus);
//Does Not Overflow
//Function * createFunctionOperatorPure(CSolver *, enum ArithOp op);
Predicate * createPredicateOperator(CSolver *solver, enum CompOp op, Set ** domain, uint numDomain);

Table * createTable(CSolver *solver, Set **domains, uint numDomain, Set * range);
void addTableEntry(CSolver *solver, uint64_t* inputs, uint inputSize, uint64_t result);
Function * completeTable(CSolver *, Table *);

Element * applyFunction(CSolver *, Function * function, Element ** array);
Boolean * applyPredicate(CSolver *, Predicate * predicate, Element ** inputs);
Boolean * applyLogicalOperation(CSolver *, enum LogicOp op, Boolean ** array);

void addBoolean(CSolver *, Boolean * constraint);

Order * createOrder(CSolver *, enum OrderType type, Set * set);
Boolean * orderConstraint(CSolver *, Order * order, uint64_t first, uint64_t second);
#endif
