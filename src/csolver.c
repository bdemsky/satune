#include "csolver.h"

CSolver * allocCSolver() {
	CSolver * tmp=(CSolver *) ourmalloc(sizeof(CSolver));
	tmp->constraint=allocVector();
	return tmp;
}

Set * createSet(CSolver * solver, Type type, uint64_t ** elements) {
	
}

Set * createSet(CSolver * solver, Type type, uint64_t lowrange, uint64_t highrange) {
}

MutableSet * createMutableSet(CSolver * solver, Type type) {
}

void CSolver::addItem(MutableSet * set, uint64_t element) {
}

int64_t CSolver::createUniqueItem(MutableSet * set) {
}

Element * CSolver::getElementVar(Set * set) {
}

Boolean * CSolver::getBooleanVar() {
}

Function * CSolver::createFunctionOperator(enum ArithOp op, Set ** domain, Set * range, enum OverFlowBehavior overflowbehavior, Boolean * overflowstatus) {
}

Function * CSolver::createFunctionOperator(enum ArithOp op) {
}

Predicate * CSolver::createPredicateOperator(enum CompOp op, Set ** domain) {
}

Table * CSolver::createTable(Set **domains, Set * range) {
}

void CSolver::addTableEntry(Element ** inputs, Element *result) {
}

Function * CSolver::completeTable(struct Table *) {
}

Element * CSolver::applyFunction(Function * function, Element ** array) {
}

Boolean * CSolver::applyPredicate(Predicate * predicate, Element ** inputs) {
}

Boolean * CSolver::applyLogicalOperation(enum LogicOp op, Boolean ** array) {
}

void CSolver::addBoolean(Boolean * constraint) {
}

Order * CSolver::createOrder(enum OrderType type, Set * set) {
}

Boolean * CSolver::orderedConstraint(Order * order, uint64_t first, uint64_t second) {
}
