#include "csolver.h"
#include "ccsolver.h"

#define CCSOLVER(solver) ((CSolver *)solver)

void *createCCSolver() {
	return (void *) new CSolver();
}
void deleteCCSolver(void *solver) {
	delete CCSOLVER(solver);
}

void *createSet(void *solver,unsigned int type, long *elements, unsigned int num) {
	return CCSOLVER(solver)->createSet((VarType) type, (uint64_t *)elements, (uint) num);
}

void *createRangeSet(void *solver,unsigned int type, long lowrange, long highrange) {
	return CCSOLVER(solver)->createRangeSet((VarType) type, (uint64_t) lowrange, (uint64_t) highrange);
}

void *createRangeVar(void *solver,unsigned int type, long lowrange, long highrange) {
	return CCSOLVER(solver)->createRangeVar((VarType) type, (uint64_t) lowrange, (uint64_t) highrange);
}

void *createMutableSet(void *solver,unsigned int type) {
	return CCSOLVER(solver)->createMutableSet((VarType) type);
}

void addItem(void *solver,void *set, long element) {
	CCSOLVER(solver)->addItem((MutableSet *) set, (uint64_t) element);
}

void finalizeMutableSet(void *solver,void *set) {
	CCSOLVER(solver)->finalizeMutableSet((MutableSet *) set);
}

void *getElementVar(void *solver,void *set) {
	return CCSOLVER(solver)->getElementVar((Set *) set);
}

void *getElementConst(void *solver,unsigned int type, long value) {
	return CCSOLVER(solver)->getElementConst((VarType) type, (uint64_t) value);
}

void *getElementRange (void *solver,void *element) {
	return CCSOLVER(solver)->getElementRange ((Element *) element);
}

void *getBooleanVar(void *solver,unsigned int type) {
	return CCSOLVER(solver)->getBooleanVar((VarType) type).getRaw();
}

void *createFunctionOperator(void *solver,unsigned int op, void *range,unsigned int overflowbehavior) {
	return CCSOLVER(solver)->createFunctionOperator((ArithOp) op, (Set *)range, (OverFlowBehavior) overflowbehavior);
}

void *createPredicateOperator(void *solver,unsigned int op) {
	return CCSOLVER(solver)->createPredicateOperator((CompOp) op);
}

void *createPredicateTable(void *solver,void *table, unsigned int behavior) {
	return CCSOLVER(solver)->createPredicateTable((Table *)table, (UndefinedBehavior) behavior);
}

void *createTable(void *solver, void *range) {
	return CCSOLVER(solver)->createTable((Set *)range);
}

void *createTableForPredicate(void *solver) {
	return CCSOLVER(solver)->createTableForPredicate();
}

void addTableEntry(void *solver,void *table, void *inputs, unsigned int inputSize, long result) {
	CCSOLVER(solver)->addTableEntry((Table *)table, (uint64_t *)inputs, (uint) inputSize, (uint64_t) result);
}

void *completeTable(void *solver,void *table, unsigned int behavior) {
	return CCSOLVER(solver)->completeTable((Table *) table, (UndefinedBehavior) behavior);
}

void *applyFunction(void *solver,void *function, void **array, unsigned int numArrays, void *overflowstatus) {
	return CCSOLVER(solver)->applyFunction((Function *)function, (Element **)array, (uint) numArrays, BooleanEdge ((Boolean *)overflowstatus));
}

void *applyPredicateTable(void *solver,void *predicate, void **inputs, unsigned int numInputs, void *undefinedStatus) {
	return CCSOLVER(solver)->applyPredicateTable((Predicate *)predicate, (Element **)inputs, (uint) numInputs, BooleanEdge((Boolean *) undefinedStatus)).getRaw();
}

void *applyPredicate(void *solver,void *predicate, void **inputs, unsigned int numInputs) {
	return CCSOLVER(solver)->applyPredicate((Predicate *)predicate, (Element **)inputs, (uint) numInputs).getRaw();
}

void *applyLogicalOperation(void *solver,unsigned int op, void *array, unsigned int asize) {
	return CCSOLVER(solver)->applyLogicalOperation((LogicOp) op, (BooleanEdge *)array, (uint) asize).getRaw();
}

void *applyLogicalOperationTwo(void *solver,unsigned int op, void *arg1, void *arg2) {
	return CCSOLVER(solver)->applyLogicalOperation((LogicOp) op, BooleanEdge((Boolean *) arg1), BooleanEdge((Boolean *) arg2)).getRaw();
}

void *applyLogicalOperationOne(void *solver,unsigned int op, void *arg) {
	return CCSOLVER(solver)->applyLogicalOperation((LogicOp) op, BooleanEdge((Boolean *) arg)).getRaw();
}

void addConstraint(void *solver,void *constraint) {
	CCSOLVER(solver)->addConstraint(BooleanEdge((Boolean *) constraint));
}

void *createOrder(void *solver,unsigned int type, void *set) {
	return CCSOLVER(solver)->createOrder((OrderType) type, (Set *)set);
}

void *orderConstraint(void *solver,void *order, long first, long second) {
	return CCSOLVER(solver)->orderConstraint((Order *)order, (uint64_t) first, (uint64_t) second).getRaw();
}

int solve(void *solver) {
	return CCSOLVER(solver)->solve();
}

long getElementValue(void *solver,void *element) {
	return (long) CCSOLVER(solver)->getElementValue((Element *)element);
}

int getBooleanValue(void *solver, void *boolean) {
	return CCSOLVER(solver)->getBooleanValue(BooleanEdge((Boolean *) boolean));
}

int getOrderConstraintValue(void *solver,void *order, long first, long second) {
	return CCSOLVER(solver)->getOrderConstraintValue((Order *)order, (uint64_t) first, (uint64_t) second);
}

void printConstraints(void *solver) {
	CCSOLVER(solver)->printConstraints();
}


void serialize(void *solver) {
	CCSOLVER(solver)->serialize();
}


void mustHaveValue(void *solver, void *element) {
	CCSOLVER(solver)->mustHaveValue( (Element *) element);
}

void setInterpreter(void *solver, unsigned int type){
	CCSOLVER(solver)->setInterpreter((InterpreterType)type);
}

void *clone(void *solver) {
	return CCSOLVER(solver)->clone();
}
