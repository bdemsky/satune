#include "csolver.h"

CSolver * allocCSolver() {
	CSolver * tmp=(CSolver *) ourmalloc(sizeof(CSolver));
	tmp->constraint=allocDefVectorBoolean();
	return tmp;
}

Set * createSet(CSolver * solver, VarType type, uint64_t ** elements) {
	return NULL;
}

Set * createRangeSet(CSolver * solver, VarType type, uint64_t lowrange, uint64_t highrange) {
	return NULL;
}

MutableSet * createMutableSet(CSolver * solver, VarType type) {
	return NULL;
}

void addItem(CSolver *solver, MutableSet * set, uint64_t element) {
}

int64_t createUniqueItem(CSolver *solver, MutableSet * set) {
	return 0;
}

Element * getElementVar(CSolver *solver, Set * set) {
	return NULL;
}

Boolean * getBooleanVar(CSolver *solver) {
	return NULL;
}

Function * createFunctionOperator(CSolver *solver, enum ArithOp op, Set ** domain, Set * range, enum OverFlowBehavior overflowbehavior, Boolean * overflowstatus) {
	return NULL;
}

Function * createFunctionOperatorPure(CSolver *solver, enum ArithOp op) {
	return NULL;
}

Predicate * createPredicateOperator(CSolver *solver, enum CompOp op, Set ** domain) {
	return NULL;
}

Table * createTable(CSolver *solver, Set **domains, Set * range) {
	return NULL;
}

void addTableEntry(CSolver *solver, Element ** inputs, Element *result) {
}

Function * completeTable(CSolver *solver, Table * table) {
	return NULL;
}

Element * applyFunction(CSolver *solver, Function * function, Element ** array) {
	return NULL;
}

Boolean * applyPredicate(CSolver *solver, Predicate * predicate, Element ** inputs) {
	return NULL;
}

Boolean * applyLogicalOperation(CSolver *solver, enum LogicOp op, Boolean ** array) {
	return NULL;
}

void addBoolean(CSolver *solver, Boolean * constraint) {
}

Order * createOrder(CSolver *solver, enum OrderType type, Set * set) {
	return NULL;
}

Boolean * orderConstraint(CSolver *solver, Order * order, uint64_t first, uint64_t second) {
	return NULL;
}
