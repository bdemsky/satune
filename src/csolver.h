#ifndef CSOLVER_H
#define CSOLVER_H
#include "classlist.h"
#include "ops.h"
#include "structs.h"

struct CSolver {
	VectorBoolean * constraints;
	VectorBoolean * allBooleans;
	VectorSet * allSets;
	VectorElement * allElements;
};

/** Create a new solver instance. */

CSolver * allocCSolver();

/** This function creates a set containing the elements passed in the array. */

Set * createSet(CSolver *, VarType type, uint64_t * elements, uint num);

/** This function creates a set from lowrange to highrange (inclusive). */

Set * createRangeSet(CSolver *, VarType type, uint64_t lowrange, uint64_t highrange);

/** This function creates a mutable set. */

MutableSet * createMutableSet(CSolver *, VarType type);

/** This function adds a new item to a set. */

void addItem(CSolver *, MutableSet * set, uint64_t element);

/** This function adds a new unique item to the set and returns it.
    This function cannot be used in conjunction with manually adding
    items to the set. */

uint64_t createUniqueItem(CSolver *, MutableSet * set);

/** This function creates an element variable over a set. */

Element * getElementVar(CSolver *, Set * set);

/** This function creates a boolean variable. */

Boolean * getBooleanVar(CSolver *, VarType type);

/** This function creates a function operator. */

Function * createFunctionOperator(CSolver *solver, enum ArithOp op, Set ** domain, uint numDomain, Set * range,
																	enum OverFlowBehavior overflowbehavior, Boolean * overflowstatus);

/** This function creates a predicate operator. */

Predicate * createPredicateOperator(CSolver *solver, enum CompOp op, Set ** domain, uint numDomain);

/** This function creates an empty instance table.*/

Table * createTable(CSolver *solver, Set **domains, uint numDomain, Set * range);

/** This function adds an input output relation to a table. */

void addTableEntry(CSolver *solver, uint64_t* inputs, uint inputSize, uint64_t result);

/** This function converts a completed table into a function. */

Function * completeTable(CSolver *, Table *);

/** This function applies a function to the Elements in its input. */

Element * applyFunction(CSolver *, Function * function, Element ** array);

/** This function applies a predicate to the Elements in its input. */

Boolean * applyPredicate(CSolver *, Predicate * predicate, Element ** inputs);

/** This function applies a logical operation to the Booleans in its input. */

Boolean * applyLogicalOperation(CSolver *, enum LogicOp op, Boolean ** array);

/** This function adds a boolean constraint to the set of constraints
    to be satisfied */

void addBoolean(CSolver *, Boolean * constraint);

/** This function instantiates an order of type type over the set set. */
Order * createOrder(CSolver *, enum OrderType type, Set * set);

/** This function instantiates a predicate on two items in an order. */
Boolean * orderConstraint(CSolver *, Order * order, uint64_t first, uint64_t second);
#endif
