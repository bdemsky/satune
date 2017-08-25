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

CSolver *allocCSolver() {
	CSolver *This = (CSolver *) ourmalloc(sizeof(CSolver));
	This->unsat = false;
	This->constraints = allocDefHashSetBoolean();
	This->allBooleans = allocDefVectorBoolean();
	This->allSets = allocDefVectorSet();
	This->allElements = allocDefVectorElement();
	This->allPredicates = allocDefVectorPredicate();
	This->allTables = allocDefVectorTable();
	This->allOrders = allocDefVectorOrder();
	This->allFunctions = allocDefVectorFunction();
	This->tuner = allocTuner();
	This->satEncoder = allocSATEncoder(This);
	return This;
}

/** This function tears down the solver and the entire AST */

void deleteSolver(CSolver *This) {
	deleteHashSetBoolean(This->constraints);

	uint size = getSizeVectorBoolean(This->allBooleans);
	for (uint i = 0; i < size; i++) {
		deleteBoolean(getVectorBoolean(This->allBooleans, i));
	}
	deleteVectorBoolean(This->allBooleans);

	size = getSizeVectorSet(This->allSets);
	for (uint i = 0; i < size; i++) {
		deleteSet(getVectorSet(This->allSets, i));
	}
	deleteVectorSet(This->allSets);

	size = getSizeVectorElement(This->allElements);
	for (uint i = 0; i < size; i++) {
		delete getVectorElement(This->allElements, i);
	}
	deleteVectorElement(This->allElements);

	size = getSizeVectorTable(This->allTables);
	for (uint i = 0; i < size; i++) {
		deleteTable(getVectorTable(This->allTables, i));
	}
	deleteVectorTable(This->allTables);

	size = getSizeVectorPredicate(This->allPredicates);
	for (uint i = 0; i < size; i++) {
		deletePredicate(getVectorPredicate(This->allPredicates, i));
	}
	deleteVectorPredicate(This->allPredicates);

	size = getSizeVectorOrder(This->allOrders);
	for (uint i = 0; i < size; i++) {
		deleteOrder(getVectorOrder(This->allOrders, i));
	}
	deleteVectorOrder(This->allOrders);

	size = getSizeVectorFunction(This->allFunctions);
	for (uint i = 0; i < size; i++) {
		deleteFunction(getVectorFunction(This->allFunctions, i));
	}
	deleteVectorFunction(This->allFunctions);
	deleteSATEncoder(This->satEncoder);
	deleteTuner(This->tuner);
	ourfree(This);
}

Set *createSet(CSolver *This, VarType type, uint64_t *elements, uint numelements) {
	Set *set = allocSet(type, elements, numelements);
	pushVectorSet(This->allSets, set);
	return set;
}

Set *createRangeSet(CSolver *This, VarType type, uint64_t lowrange, uint64_t highrange) {
	Set *set = allocSetRange(type, lowrange, highrange);
	pushVectorSet(This->allSets, set);
	return set;
}

MutableSet *createMutableSet(CSolver *This, VarType type) {
	MutableSet *set = allocMutableSet(type);
	pushVectorSet(This->allSets, set);
	return set;
}

void addItem(CSolver *This, MutableSet *set, uint64_t element) {
	addElementMSet(set, element);
}

uint64_t createUniqueItem(CSolver *This, MutableSet *set) {
	uint64_t element = set->low++;
	addElementMSet(set, element);
	return element;
}

Element *getElementVar(CSolver *This, Set *set) {
	Element *element = new ElementSet(set);
	pushVectorElement(This->allElements, element);
	return element;
}

Element *getElementConst(CSolver *This, VarType type, uint64_t value) {
	Element *element = new ElementConst(value, type);
	pushVectorElement(This->allElements, element);
	return element;
}

Boolean *getBooleanVar(CSolver *This, VarType type) {
	Boolean *boolean = allocBooleanVar(type);
	pushVectorBoolean(This->allBooleans, boolean);
	return boolean;
}

Function *createFunctionOperator(CSolver *This, ArithOp op, Set **domain, uint numDomain, Set *range,OverFlowBehavior overflowbehavior) {
	Function *function = allocFunctionOperator(op, domain, numDomain, range, overflowbehavior);
	pushVectorFunction(This->allFunctions, function);
	return function;
}

Predicate *createPredicateOperator(CSolver *This, CompOp op, Set **domain, uint numDomain) {
	Predicate *predicate = allocPredicateOperator(op, domain,numDomain);
	pushVectorPredicate(This->allPredicates, predicate);
	return predicate;
}

Predicate *createPredicateTable(CSolver *This, Table *table, UndefinedBehavior behavior) {
	Predicate *predicate = allocPredicateTable(table, behavior);
	pushVectorPredicate(This->allPredicates, predicate);
	return predicate;
}

Table *createTable(CSolver *This, Set **domains, uint numDomain, Set *range) {
	Table *table = allocTable(domains,numDomain,range);
	pushVectorTable(This->allTables, table);
	return table;
}

Table *createTableForPredicate(CSolver *solver, Set **domains, uint numDomain) {
	return createTable(solver, domains, numDomain, NULL);
}

void addTableEntry(CSolver *This, Table *table, uint64_t *inputs, uint inputSize, uint64_t result) {
	addNewTableEntry(table,inputs, inputSize,result);
}

Function *completeTable(CSolver *This, Table *table, UndefinedBehavior behavior) {
	Function *function = allocFunctionTable(table, behavior);
	pushVectorFunction(This->allFunctions,function);
	return function;
}

Element *applyFunction(CSolver *This, Function *function, Element **array, uint numArrays, Boolean *overflowstatus) {
	Element *element = new ElementFunction(function,array,numArrays,overflowstatus);
	pushVectorElement(This->allElements, element);
	return element;
}

Boolean *applyPredicate(CSolver *This, Predicate *predicate, Element **inputs, uint numInputs) {
	return applyPredicateTable(This, predicate, inputs, numInputs, NULL);
}
Boolean *applyPredicateTable(CSolver *This, Predicate *predicate, Element **inputs, uint numInputs, Boolean *undefinedStatus) {
	Boolean *boolean = new BooleanPredicate(predicate, inputs, numInputs, undefinedStatus);
	pushVectorBoolean(This->allBooleans, boolean);
	return boolean;
}

Boolean *applyLogicalOperation(CSolver *This, LogicOp op, Boolean **array, uint asize) {
	return new BooleanLogic(This, op, array, asize);
}

void addConstraint(CSolver *This, Boolean *constraint) {
	addHashSetBoolean(This->constraints, constraint);
}

Order *createOrder(CSolver *This, OrderType type, Set *set) {
	Order *order = allocOrder(type, set);
	pushVectorOrder(This->allOrders, order);
	return order;
}

Boolean *orderConstraint(CSolver *This, Order *order, uint64_t first, uint64_t second) {
	Boolean *constraint = new BooleanOrder(order, first, second);
	pushVectorBoolean(This->allBooleans,constraint);
	return constraint;
}

int startEncoding(CSolver *This) {
	naiveEncodingDecision(This);
	SATEncoder *satEncoder = This->satEncoder;
	computePolarities(This);
	orderAnalysis(This);
	encodeAllSATEncoder(This, satEncoder);
	int result = solveCNF(satEncoder->cnf);
	model_print("sat_solver's result:%d\tsolutionSize=%d\n", result, satEncoder->cnf->solver->solutionsize);
	for (int i = 1; i <= satEncoder->cnf->solver->solutionsize; i++) {
		model_print("%d, ", satEncoder->cnf->solver->solution[i]);
	}
	model_print("\n");
	return result;
}

uint64_t getElementValue(CSolver *This, Element *element) {
	switch (GETELEMENTTYPE(element)) {
	case ELEMSET:
	case ELEMCONST:
	case ELEMFUNCRETURN:
		return getElementValueSATTranslator(This, element);
	default:
		ASSERT(0);
	}
	exit(-1);
}

bool getBooleanValue( CSolver *This, Boolean *boolean) {
	switch (GETBOOLEANTYPE(boolean)) {
	case BOOLEANVAR:
		return getBooleanVariableValueSATTranslator(This, boolean);
	default:
		ASSERT(0);
	}
	exit(-1);
}

HappenedBefore getOrderConstraintValue(CSolver *This, Order *order, uint64_t first, uint64_t second) {
	return getOrderConstraintValueSATTranslator(This, order, first, second);
}

