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
#include "orderencoder.h"
#include "polarityassignment.h"

CSolver::CSolver() : unsat(false) {
	constraints = allocDefHashSetBoolean();
	allBooleans = allocDefVectorBoolean();
	allSets = allocDefVectorSet();
	allElements = allocDefVectorElement();
	allPredicates = allocDefVectorPredicate();
	allTables = allocDefVectorTable();
	allOrders = allocDefVectorOrder();
	allFunctions = allocDefVectorFunction();
	tuner = allocTuner();
	satEncoder = allocSATEncoder(this);
}

/** This function tears down the solver and the entire AST */

CSolver::~CSolver() {
	deleteHashSetBoolean(constraints);

	uint size = getSizeVectorBoolean(allBooleans);
	for (uint i = 0; i < size; i++) {
		delete getVectorBoolean(allBooleans, i);
	}
	deleteVectorBoolean(allBooleans);

	size = getSizeVectorSet(allSets);
	for (uint i = 0; i < size; i++) {
		delete getVectorSet(allSets, i);
	}
	deleteVectorSet(allSets);

	size = getSizeVectorElement(allElements);
	for (uint i = 0; i < size; i++) {
		delete getVectorElement(allElements, i);
	}
	deleteVectorElement(allElements);

	size = getSizeVectorTable(allTables);
	for (uint i = 0; i < size; i++) {
		delete getVectorTable(allTables, i);
	}
	deleteVectorTable(allTables);

	size = getSizeVectorPredicate(allPredicates);
	for (uint i = 0; i < size; i++) {
		delete getVectorPredicate(allPredicates, i);
	}
	deleteVectorPredicate(allPredicates);

	size = getSizeVectorOrder(allOrders);
	for (uint i = 0; i < size; i++) {
		delete getVectorOrder(allOrders, i);
	}
	deleteVectorOrder(allOrders);

	size = getSizeVectorFunction(allFunctions);
	for (uint i = 0; i < size; i++) {
		delete getVectorFunction(allFunctions, i);
	}
	deleteVectorFunction(allFunctions);
	deleteSATEncoder(satEncoder);
	deleteTuner(tuner);
}

Set *CSolver::createSet(VarType type, uint64_t *elements, uint numelements) {
	Set *set = new Set(type, elements, numelements);
	pushVectorSet(allSets, set);
	return set;
}

Set *CSolver::createRangeSet(VarType type, uint64_t lowrange, uint64_t highrange) {
	Set *set = new Set(type, lowrange, highrange);
	pushVectorSet(allSets, set);
	return set;
}

MutableSet *CSolver::createMutableSet(VarType type) {
	MutableSet *set = allocMutableSet(type);
	pushVectorSet(allSets, set);
	return set;
}

void CSolver::addItem(MutableSet *set, uint64_t element) {
	addElementMSet(set, element);
}

uint64_t CSolver::createUniqueItem(MutableSet *set) {
	uint64_t element = set->low++;
	addElementMSet(set, element);
	return element;
}

Element *CSolver::getElementVar(Set *set) {
	Element *element = new ElementSet(set);
	pushVectorElement(allElements, element);
	return element;
}

Element *CSolver::getElementConst(VarType type, uint64_t value) {
	Element *element = new ElementConst(value, type);
	pushVectorElement(allElements, element);
	return element;
}

Boolean *CSolver::getBooleanVar(VarType type) {
	Boolean *boolean = new BooleanVar(type);
	pushVectorBoolean(allBooleans, boolean);
	return boolean;
}

Function *CSolver::createFunctionOperator(ArithOp op, Set **domain, uint numDomain, Set *range,OverFlowBehavior overflowbehavior) {
	Function *function = new FunctionOperator(op, domain, numDomain, range, overflowbehavior);
	pushVectorFunction(allFunctions, function);
	return function;
}

Predicate *CSolver::createPredicateOperator(CompOp op, Set **domain, uint numDomain) {
	Predicate *predicate = new PredicateOperator(op, domain,numDomain);
	pushVectorPredicate(allPredicates, predicate);
	return predicate;
}

Predicate *CSolver::createPredicateTable(Table *table, UndefinedBehavior behavior) {
	Predicate *predicate = new PredicateTable(table, behavior);
	pushVectorPredicate(allPredicates, predicate);
	return predicate;
}

Table *CSolver::createTable(Set **domains, uint numDomain, Set *range) {
	Table *table = new Table(domains,numDomain,range);
	pushVectorTable(allTables, table);
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
	pushVectorFunction(allFunctions,function);
	return function;
}

Element *CSolver::applyFunction(Function *function, Element **array, uint numArrays, Boolean *overflowstatus) {
	Element *element = new ElementFunction(function,array,numArrays,overflowstatus);
	pushVectorElement(allElements, element);
	return element;
}

Boolean *CSolver::applyPredicate(Predicate *predicate, Element **inputs, uint numInputs) {
	return applyPredicateTable(predicate, inputs, numInputs, NULL);
}

Boolean *CSolver::applyPredicateTable(Predicate *predicate, Element **inputs, uint numInputs, Boolean *undefinedStatus) {
	Boolean *boolean = new BooleanPredicate(predicate, inputs, numInputs, undefinedStatus);
	pushVectorBoolean(allBooleans, boolean);
	return boolean;
}

Boolean *CSolver::applyLogicalOperation(LogicOp op, Boolean **array, uint asize) {
	return new BooleanLogic(this, op, array, asize);
}

void CSolver::addConstraint(Boolean *constraint) {
	addHashSetBoolean(constraints, constraint);
}

Order *CSolver::createOrder(OrderType type, Set *set) {
	Order *order = new Order(type, set);
	pushVectorOrder(allOrders, order);
	return order;
}

Boolean *CSolver::orderConstraint(Order *order, uint64_t first, uint64_t second) {
	Boolean *constraint = new BooleanOrder(order, first, second);
	pushVectorBoolean(allBooleans,constraint);
	return constraint;
}

int CSolver::startEncoding() {
	naiveEncodingDecision(this);
	computePolarities(this);
	orderAnalysis(this);
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

