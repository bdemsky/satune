#ifndef CSOLVER_H
#define CSOLVER_H
#include "classlist.h"
#include "ops.h"
#include "structs.h"

struct CSolver {
	VectorBoolean * constraint;
        uint64_t uniqSec;
};
	
CSolver * allocCSolver();
Set * createSet(CSolver *, VarType type, uint64_t * elements, uint num);
Set * createRangeSet(CSolver *, VarType type, uint64_t lowrange, uint64_t highrange);
MutableSet * createMutableSet(CSolver *, VarType type);

void addItem(CSolver *, MutableSet * set, uint64_t element);
int64_t createUniqueItem(CSolver *, MutableSet * set);

Element * getElementVar(CSolver *, Set * set);
Boolean * getBooleanVar(CSolver *);

Function * createFunctionOperator(CSolver *, enum ArithOp op, Set ** domain, Set * range, enum OverFlowBehavior overflowbehavior, Boolean * overflowstatus);
//Does Not Overflow
Function * createFunctionOperatorPure(CSolver *, enum ArithOp op);
Predicate * createPredicateOperator(CSolver *, enum CompOp op, Set ** domain);

Table * createTable(CSolver *, Set **domains, Set * range);
void addTableEntry(CSolver *, Element ** inputs, Element *result);
Function * completeTable(CSolver *, struct Table *);

Element * applyFunction(CSolver *, Function * function, Element ** array);
Boolean * applyPredicate(CSolver *, Predicate * predicate, Element ** inputs);
Boolean * applyLogicalOperation(CSolver *, enum LogicOp op, Boolean ** array);

void addBoolean(CSolver *, Boolean * constraint);

Order * createOrder(CSolver *, enum OrderType type, Set * set);
Boolean * orderConstraint(CSolver *, Order * order, uint64_t first, uint64_t second);
#endif
