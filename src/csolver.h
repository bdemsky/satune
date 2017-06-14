#ifndef CSOLVER_H
#define CSOLVER_H
#include "classlist.h"
#include "ops.h"

class CSolver {
public:
	CSolver();
	Set * createSet(Type type, uint64_t ** elements);
	Set * createSet(Type type, uint64_t lowrange, uint64_t highrange);
	MutableSet * createMutableSet(Type type);

	void addItem(MutableSet * set, uint64_t element);
	int64_t createUniqueItem(MutableSet * set);

	Element * getElementVar(Set * set);
	Constraint * getBooleanVar();

	Function * createFunctionOperator(enum ArithOp op, Set ** domain, Set * range, enum OverFlowBehavior overflowbehavior, Constraint * overflowstatus);
	//Does Not Overflow
	Function * createFunctionOperator(enum ArithOp op);
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
private:
	ModelVector<Constraint *> constraints;
};
#endif
