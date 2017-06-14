#ifndef CSOLVER_H
#define CSOLVER_H
#include "classlist.h"
#include "ops.h"

class CSolver {
	Set * createSet(Type type, uint64_t ** elements);
	Set * createSet(Type type, uint64_t lowrange, uint64_t highrange);
	MutableSet * createMutableSet(Type type);

	void addItem(MutableSet * set, uint64_t element);
	int64_t createUniqueItem(MutableSet * set);

	Element * getElementVar(Set * set);
	Constraint * getBooleanVar();

	Function * createFunctionOperator(enum ArithOp op, Set ** domain, Set * range, enum OverFlowBehavior overflowbehavior, Constraint * overflowstatus);
	Function * createFunctionOperator(enum ArithOp op);														//Does Not Overflow
	Predicate * createPredicateOperator(enum CompOp op, Set ** domain);

	Table * createTable(Set **domains, Set * range);
	void addTableEntry(Element ** inputs, Element *result);
	Function * completeTable(struct Table *);

	Element * applyFunction(Function * function, Element ** array);
	Constraint * applyPredicate(Predicate * predicate, Element ** inputs);
	Constraint * applyLogicalOperation(enum LogicOp op, Constraint ** array);

	void addConstraint(Constraint * constraint);

	Order * createOrder(enum OrderType type, Set * set);
	Constraint * orderedConstraint(Order * order, uint64_t first, uint64_t second);
};
#endif
