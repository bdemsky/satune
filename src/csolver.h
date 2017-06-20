#ifndef CSOLVER_H
#define CSOLVER_H
#include "classlist.h"
#include "ops.h"
#include "structs.h"

struct CSolver {
	/** This is a vector of constraints that must be satisfied. */
	VectorBoolean * constraints;

	/** This is a vector of all boolean structs that we have allocated. */
	VectorBoolean * allBooleans;

	/** This is a vector of all set structs that we have allocated. */
	VectorSet * allSets;

	/** This is a vector of all element structs that we have allocated. */
	VectorElement * allElements;
	
	/** This is a vector of all predicate structs that we have allocated. */
	VectorPredicate * allPredicates;
	
	/** This is a vector of all table structs that we have allocated. */
	VectorTable * allTables;
	
	/** This is a vector of all order structs that we have allocated. */
	VectorOrder * allOrders;
	
	/** This is a vector of all function structs that we have allocated. */
	VectorFunction* allFunctions;
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

Function * createFunctionOperator(CSolver *solver, ArithOp op, Set ** domain, uint numDomain, Set * range,
																	OverFlowBehavior overflowbehavior);

/** This function creates a predicate operator. */

Predicate * createPredicateOperator(CSolver *solver, CompOp op, Set ** domain, uint numDomain);

/** This function creates an empty instance table.*/

Table * createTable(CSolver *solver, Set **domains, uint numDomain, Set * range);

/** This function adds an input output relation to a table. */

void addTableEntry(CSolver *solver, Table* table, uint64_t* inputs, uint inputSize, uint64_t result);

/** This function converts a completed table into a function. */

Function * completeTable(CSolver *, Table *);

/** This function applies a function to the Elements in its input. */

Element * applyFunction(CSolver *, Function * function, Element ** array, uint numArrays, Boolean * overflowstatus);

/** This function applies a predicate to the Elements in its input. */

Boolean * applyPredicate(CSolver *, Predicate * predicate, Element ** inputs, uint numInputs);

/** This function applies a logical operation to the Booleans in its input. */

Boolean * applyLogicalOperation(CSolver *, LogicOp op, Boolean ** array, uint asize);

/** This function adds a boolean constraint to the set of constraints
    to be satisfied */

void addBoolean(CSolver *, Boolean * constraint);

/** This function instantiates an order of type type over the set set. */
Order * createOrder(CSolver *, OrderType type, Set * set);

/** This function instantiates a boolean on two items in an order. */
Boolean * orderConstraint(CSolver *, Order * order, uint64_t first, uint64_t second);
#endif
