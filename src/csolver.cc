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
#include "transformer.h"
#include "autotuner.h"
#include "astops.h"
#include "structs.h"

CSolver::CSolver() :
	boolTrue(new BooleanConst(true)),
	boolFalse(new BooleanConst(false)),
	unsat(false),
	tuner(NULL),
	elapsedTime(0)
{
	satEncoder = new SATEncoder(this);
	transformer = new Transformer(this);
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

	delete boolTrue;
	delete boolFalse;
	delete satEncoder;
	delete transformer;
}

CSolver *CSolver::clone() {
	CSolver *copy = new CSolver();
	CloneMap map;
	SetIteratorBoolean *it = getConstraints();
	while (it->hasNext()) {
		Boolean *b = it->next();
		copy->addConstraint(b->clone(copy, &map));
	}
	delete it;
	return copy;
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
	uint64_t element = set->getNewUniqueItem();
	set->addElementMSet(element);
	return element;
}

Element *CSolver::getElementVar(Set *set) {
	Element *element = new ElementSet(set);
	allElements.push(element);
	return element;
}

Element *CSolver::getElementConst(VarType type, uint64_t value) {
	uint64_t array[] = {value};
	Set *set = new Set(type, array, 1);
	Element *element = new ElementConst(value, type, set);
	Element *e = elemMap.get(element);
	if (e == NULL) {
		allSets.push(set);
		allElements.push(element);
		elemMap.put(element, element);
		return element;
	} else {
		delete set;
		delete element;
		return e;
	}
}

Element *CSolver::applyFunction(Function *function, Element **array, uint numArrays, Boolean *overflowstatus) {
	Element *element = new ElementFunction(function,array,numArrays,overflowstatus);
	Element *e = elemMap.get(element);
	if (e == NULL) {
		allElements.push(element);
		elemMap.put(element, element);
		return element;
	} else {
		delete element;
		return e;
	}
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

Boolean *CSolver::getBooleanVar(VarType type) {
	Boolean *boolean = new BooleanVar(type);
	allBooleans.push(boolean);
	return boolean;
}

Boolean *CSolver::getBooleanTrue() {
	return boolTrue;
}

Boolean *CSolver::getBooleanFalse() {
	return boolFalse;
}

Boolean *CSolver::applyPredicate(Predicate *predicate, Element **inputs, uint numInputs) {
	return applyPredicateTable(predicate, inputs, numInputs, NULL);
}

Boolean *CSolver::applyPredicateTable(Predicate *predicate, Element **inputs, uint numInputs, Boolean *undefinedStatus) {
	BooleanPredicate *boolean = new BooleanPredicate(predicate, inputs, numInputs, undefinedStatus);
	Boolean * b = boolMap.get(boolean);
	if (b == NULL) {
		boolMap.put(boolean, boolean);
		allBooleans.push(boolean);
		return boolean;
	} else {
		delete boolean;
		return b;
	}
}

Boolean *CSolver::applyLogicalOperation(LogicOp op, Boolean **array, uint asize) {
	Boolean * newarray[asize];
	switch(op) {
	case SATC_NOT: {
		if (array[0]->type == LOGICOP && ((BooleanLogic *)array[0])->op==SATC_NOT) {
			return ((BooleanLogic *) array[0])->inputs.get(0);
		} else if (array[0]->type == BOOLCONST) {
			bool isTrue = ((BooleanConst *) array[0])->isTrue;
			return isTrue ? boolFalse : boolTrue;
		}
		break;
	}
	case SATC_XOR: {
		for(uint i=0;i<2;i++) {
			if (array[i]->type == BOOLCONST) {
				bool isTrue = ((BooleanConst *) array[i])->isTrue;
				if (isTrue) {
					newarray[0]=array[1-i];
					return applyLogicalOperation(SATC_NOT, newarray, 1);
				} else
					return array[1-i];
			}
		}
		break;
	}
	case SATC_OR: {
		uint newindex=0;
		for(uint i=0;i<asize;i++) {
			Boolean *b=array[i];
			if (b->type == BOOLCONST) {
				bool isTrue = ((BooleanConst *) b)->isTrue;
				if (isTrue)
					return b;
				else
					continue;
			} else
				newarray[newindex++]=b;
		}
		if (newindex==1)
			return newarray[0];
		else if (newindex == 2) {
			bool isNot0 = (newarray[0]->type==BOOLCONST) && ((BooleanLogic *)newarray[0])->op == SATC_NOT;
			bool isNot1 = (newarray[1]->type==BOOLCONST) && ((BooleanLogic *)newarray[1])->op == SATC_NOT;

			if (isNot0 != isNot1) {
				if (isNot0) {
					newarray[0] = ((BooleanLogic *) newarray[0])->inputs.get(0);
				} else {
					Boolean *tmp =  ((BooleanLogic *) array[1])->inputs.get(0);
					array[1] = array[0];
					array[0] = tmp;
				}
				return applyLogicalOperation(SATC_IMPLIES, newarray, 2);
			}
		} else {
			array = newarray;
			asize = newindex;
		}
		break;
	}
	case SATC_AND: {
		uint newindex=0;
		for(uint i=0;i<asize;i++) {
			Boolean *b=array[i];
			if (b->type == BOOLCONST) {
				bool isTrue = ((BooleanConst *) b)->isTrue;
				if (isTrue)
					continue;
				else
					return b;
			} else
				newarray[newindex++]=b;
		}
		if(newindex==1) {
			return newarray[0];
		} else {
			array = newarray;
			asize = newindex;
		}
		break;
	}
	case SATC_IMPLIES: {
		if (array[0]->type == BOOLCONST) {
			BooleanConst *b=(BooleanConst *) array[0];
			if (b->isTrue) {
				return array[1];
			} else {
				return boolTrue;
			}
		} else if (array[1]->type == BOOLCONST) {
			BooleanConst *b=(BooleanConst *) array[0];
			if (b->isTrue) {
				return b;
			} else {
				return applyLogicalOperation(SATC_NOT, array, 1);
			}
		}
		break;
	}
	}
	
	Boolean *boolean = new BooleanLogic(this, op, array, asize);
	Boolean *b = boolMap.get(boolean);
	if (b == NULL) {
		boolMap.put(boolean, boolean);
		allBooleans.push(boolean);
		return boolean;		
	} else {
		delete boolean;
		return b;
	}
}

Boolean *CSolver::orderConstraint(Order *order, uint64_t first, uint64_t second) {
	Boolean *constraint = new BooleanOrder(order, first, second);
	allBooleans.push(constraint);
	return constraint;
}

void CSolver::addConstraint(Boolean *constraint) {
	if (constraint == boolTrue)
		return;
	else if (constraint == boolFalse)
		setUnSAT();
	else
		constraints.add(constraint);
}

Order *CSolver::createOrder(OrderType type, Set *set) {
	Order *order = new Order(type, set);
	allOrders.push(order);
	return order;
}

int CSolver::startEncoding() {
	bool deleteTuner = false;
	if (tuner == NULL) {
		tuner = new DefaultTuner();
		deleteTuner = true;
	}
		
	long long startTime = getTimeNano();
	computePolarities(this);
	transformer->orderAnalysis();
	naiveEncodingDecision(this);
	satEncoder->encodeAllSATEncoder(this);
	int result = unsat ? IS_UNSAT : satEncoder->solve();
	long long finishTime = getTimeNano();
	elapsedTime = finishTime - startTime;
	if (deleteTuner) {
		delete tuner;
		tuner = NULL;
	}
	return result;
}

uint64_t CSolver::getElementValue(Element *element) {
	switch (element->type) {
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
	switch (boolean->type) {
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

long long CSolver::getEncodeTime() { return satEncoder->getEncodeTime(); }

long long CSolver::getSolveTime() { return satEncoder->getSolveTime(); }

void CSolver::autoTune(uint budget) {
	AutoTuner * autotuner=new AutoTuner(budget);
	autotuner->addProblem(this);
	autotuner->tune();
	delete autotuner;
}
