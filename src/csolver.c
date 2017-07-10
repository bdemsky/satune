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

CSolver * allocCSolver() {
	CSolver * tmp=(CSolver *) ourmalloc(sizeof(CSolver));
	tmp->constraints=allocDefVectorBoolean();
	tmp->allBooleans=allocDefVectorBoolean();
	tmp->allSets=allocDefVectorSet();
	tmp->allElements=allocDefVectorElement();
	tmp->allPredicates = allocDefVectorPredicate();
	tmp->allTables = allocDefVectorTable();
	tmp->allOrders = allocDefVectorOrder();
	tmp->allFunctions = allocDefVectorFunction();
	return tmp;
}

/** This function tears down the solver and the entire AST */

void deleteSolver(CSolver *This) {
	deleteVectorBoolean(This->constraints);

	uint size=getSizeVectorBoolean(This->allBooleans);
	for(uint i=0;i<size;i++) {
		deleteBoolean(getVectorBoolean(This->allBooleans, i));
	}
	deleteVectorBoolean(This->allBooleans);

	size=getSizeVectorSet(This->allSets);
	for(uint i=0;i<size;i++) {
		deleteSet(getVectorSet(This->allSets, i));
	}
	deleteVectorSet(This->allSets);

	size=getSizeVectorElement(This->allElements);
	for(uint i=0;i<size;i++) {
		deleteElement(getVectorElement(This->allElements, i));
	}
	deleteVectorElement(This->allElements);

	size=getSizeVectorTable(This->allTables);
	for(uint i=0;i<size;i++) {
		deleteTable(getVectorTable(This->allTables, i));
	}
	deleteVectorTable(This->allTables);

	size=getSizeVectorPredicate(This->allPredicates);
	for(uint i=0;i<size;i++) {
		deletePredicate(getVectorPredicate(This->allPredicates, i));
	}
	deleteVectorPredicate(This->allPredicates);

	size=getSizeVectorOrder(This->allOrders);
	for(uint i=0;i<size;i++) {
		deleteOrder(getVectorOrder(This->allOrders, i));
	}
	deleteVectorOrder(This->allOrders);

	size=getSizeVectorFunction(This->allFunctions);
	for(uint i=0;i<size;i++) {
		deleteFunction(getVectorFunction(This->allFunctions, i));
	}
	deleteVectorFunction(This->allFunctions);
	ourfree(This);
}

Set * createSet(CSolver * This, VarType type, uint64_t * elements, uint numelements) {
	Set * set=allocSet(type, elements, numelements);
	pushVectorSet(This->allSets, set);
	return set;
}

Set * createRangeSet(CSolver * This, VarType type, uint64_t lowrange, uint64_t highrange) {
	Set * set=allocSetRange(type, lowrange, highrange);
	pushVectorSet(This->allSets, set);
	return set;
}

MutableSet * createMutableSet(CSolver * This, VarType type) {
	MutableSet * set=allocMutableSet(type);
	pushVectorSet(This->allSets, set);
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

Element * getElementVar(CSolver *This, Set * set) {
	Element * element=allocElementSet(set);
	pushVectorElement(This->allElements, element);
	return element;
}

Boolean * getBooleanVar(CSolver *solver, VarType type) {
	Boolean* boolean= allocBoolean(type);
	pushVectorBoolean(solver->allBooleans, boolean);
	return boolean;
}

Function * createFunctionOperator(CSolver *solver, ArithOp op, Set ** domain, uint numDomain, Set * range,OverFlowBehavior overflowbehavior) {
	Function* function = allocFunctionOperator(op, domain, numDomain, range, overflowbehavior);
	pushVectorFunction(solver->allFunctions, function);
	return function;
}

Predicate * createPredicateOperator(CSolver *solver, CompOp op, Set ** domain, uint numDomain) {
	Predicate* predicate= allocPredicateOperator(op, domain,numDomain);
	pushVectorPredicate(solver->allPredicates, predicate);
	return predicate;
}

Table * createTable(CSolver *solver, Set **domains, uint numDomain, Set * range) {
	Table* table= allocTable(domains,numDomain,range);
	pushVectorTable(solver->allTables, table);
	return table;
}

void addTableEntry(CSolver *solver, Table* table, uint64_t* inputs, uint inputSize, uint64_t result) {
	addNewTableEntry(table,inputs, inputSize,result);
}

Function * completeTable(CSolver *solver, Table * table) {
	Function* function = allocFunctionTable(table);
	pushVectorFunction(solver->allFunctions,function);
	return function;
}

Element * applyFunction(CSolver *solver, Function * function, Element ** array, uint numArrays, Boolean * overflowstatus) {
	Element* element= allocElementFunction(function,array,numArrays,overflowstatus);
	pushVectorElement(solver->allElements, element);
	return element;
}

Boolean * applyPredicate(CSolver *solver, Predicate * predicate, Element ** inputs, uint numInputs) {
	Boolean* boolean= allocBooleanPredicate(predicate, inputs, numInputs);
	pushVectorBoolean(solver->allBooleans, boolean);
	return boolean;
}

Boolean * applyLogicalOperation(CSolver *solver, LogicOp op, Boolean ** array, uint asize) {
	return allocBooleanLogicArray(solver, op, array, asize);
}

void addBoolean(CSolver *This, Boolean * constraint) {
	pushVectorBoolean(This->constraints, constraint);
}

Order * createOrder(CSolver *solver, OrderType type, Set * set) {
	Order* order = allocOrder(type, set);
	pushVectorOrder(solver->allOrders, order);
	return order;
}

Boolean * orderConstraint(CSolver *solver, Order * order, uint64_t first, uint64_t second) {
	Boolean* constraint = allocBooleanOrder(order, first, second);
	pushVectorBoolean(solver->allBooleans,constraint);
	return constraint;
}

void startEncoding(CSolver* solver){
	naiveEncodingDecision(solver);
	SATEncoder* satEncoder = allocSATEncoder();
	createSolver(satEncoder->satSolver);
	encodeAllSATEncoder(solver, satEncoder);
	finishedClauses(satEncoder->satSolver);
	solve(satEncoder->satSolver);
	int result= getSolution(satEncoder->satSolver);
	model_print("sat_solver's result:%d\n", result);
	killSolver(satEncoder->satSolver);
	//For now, let's just delete it, and in future for doing queries 
	//we may need it.
	deleteSATEncoder(satEncoder);
}