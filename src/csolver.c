#include "csolver.h"
#include "set.h"
#include "mutableset.h"
#include "element.h"
#include "boolean.h"
#include "predicate.h"
#include "order.h"

CSolver * allocCSolver() {
	CSolver * tmp=(CSolver *) ourmalloc(sizeof(CSolver));
	tmp->constraints=allocDefVectorBoolean();
	tmp->sets=allocDefVectorSet();
	tmp->elements=allocDefVectorElement();
	return tmp;
}

/** This function tears down the solver and the entire AST */

void deleteSolver(CSolver *this) {
	deleteVectorBoolean(this->constraints);
	uint size=getSizeVectorSet(this->sets);
	for(uint i=0;i<size;i++) {
		deleteSet(getVectorSet(this->sets, i));
	}

	deleteVectorSet(this->sets);

	size=getSizeVectorElement(this->elements);
	for(uint i=0;i<size;i++) {
		deleteElement(getVectorElement(this->elements, i));
	}

	deleteVectorElement(this->elements);
	ourfree(this);
}

Set * createSet(CSolver * this, VarType type, uint64_t * elements, uint numelements) {
	Set * set=allocSet(type, elements, numelements);
	pushVectorSet(this->sets, set);
	return set;
}

Set * createRangeSet(CSolver * this, VarType type, uint64_t lowrange, uint64_t highrange) {
	Set * set=allocSetRange(type, lowrange, highrange);
	pushVectorSet(this->sets, set);
	return set;
}

MutableSet * createMutableSet(CSolver * this, VarType type) {
	MutableSet * set=allocMutableSet(type);
	pushVectorSet(this->sets, set);
	return set;
}

void addItem(CSolver *solver, MutableSet * set, uint64_t element) {
	addElementMSet(set, element);
}

uint64_t createUniqueItem(CSolver *solver, MutableSet * set) {
	uint64_t element=set->low++;
	addElementMSet(set, element);
	return element;
}

Element * getElementVar(CSolver *this, Set * set) {
	Element * element=allocElement(set);
	pushVectorElement(this->elements, element);
	return element;
}

Boolean * getBooleanVar(CSolver *solver, VarType type) {
    Boolean* boolean= allocBoolean(type);
    pushVectorBoolean(solver->constraints, boolean);
    return boolean;
}

Function * createFunctionOperator(CSolver *solver, enum ArithOp op, Set ** domain, uint numDomain, Set * range,
        enum OverFlowBehavior overflowbehavior, Boolean * overflowstatus) {
	return NULL;
}

//Function * createFunctionOperatorPure(CSolver *solver, enum ArithOp op) {
//	return NULL;
//}

Predicate * createPredicateOperator(CSolver *solver, enum CompOp op, Set ** domain, uint numDomain) {
    return allocPredicate(op, domain,numDomain);
}

Table * createTable(CSolver *solver, Set **domains, uint numDomain, Set * range) {
	return NULL;
}

void addTableEntry(CSolver *solver, uint64_t* inputs, uint inputSize, uint64_t result) {
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

void addBoolean(CSolver *this, Boolean * constraint) {
	pushVectorBoolean(this->constraints, constraint);
}

Order * createOrder(CSolver *solver, enum OrderType type, Set * set) {
    return allocOrder(type, set);
}

Boolean * orderConstraint(CSolver *solver, Order * order, uint64_t first, uint64_t second) {
    Boolean* constraint = allocBooleanOrder(order, first, second);
    pushVectorBoolean(solver->constraints,constraint);
    return constraint;
}
