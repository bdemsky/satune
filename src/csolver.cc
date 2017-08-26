#include "csolver.h"
#include "set.h"
#include "mutableset.h"
#include "element.h"
#include "boolean.h"
#include "predicate.h"
#include "order.h"
#include "table.h"
#include "function.h"
#include "satencoder.h"
#include "sattranslator.h"
#include "tunable.h"
#include "polarityassignment.h"
#include "orderdecompose.h"

CSolver::CSolver() : unsat(false) {
	tuner = new Tuner();
	satEncoder = allocSATEncoder(this);
}

/** This function tears down the solver and the entire AST */

CSolver::~CSolver() {
	uint size = allBooleans.getSize();
	for (uint i = 0; i < size; i++) {
		delete allBooleans.get(i);
	}

	size = allSets.getSize();
	for (uint i = 0; i < size; i++) {
		delete allSets.get(i);
	}

	size = allElements.getSize();
	for (uint i = 0; i < size; i++) {
		delete allElements.get(i);
	}

	size = allTables.getSize();
	for (uint i = 0; i < size; i++) {
		delete allTables.get(i);
	}

	size = allPredicates.getSize();
	for (uint i = 0; i < size; i++) {
		delete allPredicates.get(i);
	}

	size = allOrders.getSize();
	for (uint i = 0; i < size; i++) {
		delete allOrders.get(i);
	}

	size = allFunctions.getSize();
	for (uint i = 0; i < size; i++) {
		delete allFunctions.get(i);
	}

	deleteSATEncoder(satEncoder);
	delete tuner;
}

Set *CSolver::createSet(VarType type, uint64_t *elements, uint numelements) {
	Set *set = new Set(type, elements, numelements);
	allSets.push(set);
	return set;
}

Set *CSolver::createRangeSet(VarType type, uint64_t lowrange, uint64_t highrange) {
	Set *set = new Set(type, lowrange, highrange);
	allSets.push(set);
	return set;
}

MutableSet *CSolver::createMutableSet(VarType type) {
	MutableSet *set = new MutableSet(type);
	allSets.push(set);
	return set;
}

void CSolver::addItem(MutableSet *set, uint64_t element) {
	set->addElementMSet(element);
}

uint64_t CSolver::createUniqueItem(MutableSet *set) {
	uint64_t element = set->low++;
	set->addElementMSet(element);
	return element;
}

Element *CSolver::getElementVar(Set *set) {
	Element *element = new ElementSet(set);
	allElements.push(element);
	return element;
}

Element *CSolver::getElementConst(VarType type, uint64_t value) {
	Element *element = new ElementConst(value, type);
	allElements.push(element);
	return element;
}

Boolean *CSolver::getBooleanVar(VarType type) {
	Boolean *boolean = new BooleanVar(type);
	allBooleans.push(boolean);
	return boolean;
}

Function *CSolver::createFunctionOperator(ArithOp op, Set **domain, uint numDomain, Set *range,OverFlowBehavior overflowbehavior) {
	Function *function = new FunctionOperator(op, domain, numDomain, range, overflowbehavior);
	allFunctions.push(function);
	return function;
}

Predicate *CSolver::createPredicateOperator(CompOp op, Set **domain, uint numDomain) {
	Predicate *predicate = new PredicateOperator(op, domain,numDomain);
	allPredicates.push(predicate);
	return predicate;
}

Predicate *CSolver::createPredicateTable(Table *table, UndefinedBehavior behavior) {
	Predicate *predicate = new PredicateTable(table, behavior);
	allPredicates.push(predicate);
	return predicate;
}

Table *CSolver::createTable(Set **domains, uint numDomain, Set *range) {
	Table *table = new Table(domains,numDomain,range);
	allTables.push(table);
	return table;
}

Table *CSolver::createTableForPredicate(Set **domains, uint numDomain) {
	return createTable(domains, numDomain, NULL);
}

void CSolver::addTableEntry(Table *table, uint64_t *inputs, uint inputSize, uint64_t result) {
	table->addNewTableEntry(inputs, inputSize, result);
}

Function *CSolver::completeTable(Table *table, UndefinedBehavior behavior) {
	Function *function = new FunctionTable(table, behavior);
	allFunctions.push(function);
	return function;
}

Element *CSolver::applyFunction(Function *function, Element **array, uint numArrays, Boolean *overflowstatus) {
	Element *element = new ElementFunction(function,array,numArrays,overflowstatus);
	allElements.push(element);
	return element;
}

Boolean *CSolver::applyPredicate(Predicate *predicate, Element **inputs, uint numInputs) {
	return applyPredicateTable(predicate, inputs, numInputs, NULL);
}

Boolean *CSolver::applyPredicateTable(Predicate *predicate, Element **inputs, uint numInputs, Boolean *undefinedStatus) {
	BooleanPredicate *boolean = new BooleanPredicate(predicate, inputs, numInputs, undefinedStatus);
	allBooleans.push(boolean);
	return boolean;
}

Boolean *CSolver::applyLogicalOperation(LogicOp op, Boolean **array, uint asize) {
	return new BooleanLogic(this, op, array, asize);
}

void CSolver::addConstraint(Boolean *constraint) {
	constraints.add(constraint);
}

Order *CSolver::createOrder(OrderType type, Set *set) {
	Order *order = new Order(type, set);
	allOrders.push(order);
	return order;
}

Boolean *CSolver::orderConstraint(Order *order, uint64_t first, uint64_t second) {
	Boolean *constraint = new BooleanOrder(order, first, second);
	allBooleans.push(constraint);
	return constraint;
}

int CSolver::startEncoding() {
	computePolarities(this);
	orderAnalysis(this);
	naiveEncodingDecision(this);
	encodeAllSATEncoder(this, satEncoder);
	int result = solveCNF(satEncoder->cnf);
	model_print("sat_solver's result:%d\tsolutionSize=%d\n", result, satEncoder->cnf->solver->solutionsize);
	for (int i = 1; i <= satEncoder->cnf->solver->solutionsize; i++) {
		model_print("%d, ", satEncoder->cnf->solver->solution[i]);
	}
	model_print("\n");
	return result;
}

uint64_t CSolver::getElementValue(Element *element) {
	switch (GETELEMENTTYPE(element)) {
	case ELEMSET:
	case ELEMCONST:
	case ELEMFUNCRETURN:
		return getElementValueSATTranslator(this, element);
	default:
		ASSERT(0);
	}
	exit(-1);
}

bool CSolver::getBooleanValue(Boolean *boolean) {
	switch (GETBOOLEANTYPE(boolean)) {
	case BOOLEANVAR:
		return getBooleanVariableValueSATTranslator(this, boolean);
	default:
		ASSERT(0);
	}
	exit(-1);
}

HappenedBefore CSolver::getOrderConstraintValue(Order *order, uint64_t first, uint64_t second) {
	return getOrderConstraintValueSATTranslator(this, order, first, second);
}

