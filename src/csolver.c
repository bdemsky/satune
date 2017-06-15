#include "csolver.h"
#include "set.h"
#include "mutableset.h"
#include "element.h"
CSolver * allocCSolver() {
	CSolver * tmp=(CSolver *) ourmalloc(sizeof(CSolver));
	tmp->constraint=allocDefVectorBoolean();
        tmp->uniqSec=1;
	return tmp;
}

Set * createSet(CSolver * solver, VarType type, uint64_t * elements, uint num) {
    Set* set = allocSet(type, elements,num );
    return set;
}

Set * createRangeSet(CSolver * solver, VarType type, uint64_t lowrange, uint64_t highrange) {
    Set* rset = allocSetRange(type, lowrange, highrange);
    return rset;
}

MutableSet * createMutableSet(CSolver * solver, VarType type) {
    MutableSet* mset = allocMutableSet(type);
    return mset;
}

void addItem(CSolver *solver, MutableSet * set, uint64_t element) {
    addElementMSet(set, element);
}

int64_t createUniqueItem(CSolver *solver, MutableSet * set) {
    uint64_t uSec= solver->uniqSec++;
    addElementMSet(set, uSec);
    return uSec;
}

Element * getElementVar(CSolver *solver, Set * set) {
    return allocElement(set);
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
    solver->
}

Order * createOrder(CSolver *solver, enum OrderType type, Set * set) {
	return NULL;
}

Boolean * orderConstraint(CSolver *solver, Order * order, uint64_t first, uint64_t second) {
	return NULL;
}
