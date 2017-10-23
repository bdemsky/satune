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
#include "decomposeordertransform.h"
#include "autotuner.h"
#include "astops.h"
#include "structs.h"
#include "orderresolver.h"
#include "integerencoding.h"
#include "qsort.h"
#include "preprocess.h"
#include "serializer.h"
#include "deserializer.h"
#include "encodinggraph.h"

CSolver::CSolver() :
	boolTrue(BooleanEdge(new BooleanConst(true))),
	boolFalse(boolTrue.negate()),
	unsat(false),
	tuner(NULL),
	elapsedTime(0)
{
	satEncoder = new SATEncoder(this);
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
                Element* el = allElements.get(i);
                model_print("deleting ...%u", i);
                ASSERT(el != NULL);
		delete el;
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

	delete boolTrue.getBoolean();
	delete satEncoder;
}

CSolver *CSolver::clone() {
	CSolver *copy = new CSolver();
	CloneMap map;
	SetIteratorBooleanEdge *it = getConstraints();
	while (it->hasNext()) {
		BooleanEdge b = it->next();
		copy->addConstraint(cloneEdge(copy, &map, b));
	}
	delete it;
	return copy;
}

void CSolver::serialize() {
	model_print("serializing ...\n");
	{
		Serializer serializer("dump");
		SetIteratorBooleanEdge *it = getConstraints();
		while (it->hasNext()) {
			BooleanEdge b = it->next();
			serializeBooleanEdge(&serializer, b);
		}
		delete it;
	}
//	model_print("deserializing ...\n");
//	{
//		Deserializer deserializer("dump");
//		deserializer.deserialize();
//	}
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

VarType CSolver::getSetVarType(Set *set) {
	return set->getType();
}

Element *CSolver::createRangeVar(VarType type, uint64_t lowrange, uint64_t highrange) {
	Set *s = createRangeSet(type, lowrange, highrange);
	return getElementVar(s);
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

void CSolver::finalizeMutableSet(MutableSet *set) {
	set->finalize();
}

Element *CSolver::getElementVar(Set *set) {
	Element *element = new ElementSet(set);
        model_println("%%%%ElementVar:%u", allElements.getSize());
	allElements.push(element);
	return element;
}

Set *CSolver::getElementRange (Element *element) {
	return element->getRange();
}


Element *CSolver::getElementConst(VarType type, uint64_t value) {
	uint64_t array[] = {value};
	Set *set = new Set(type, array, 1);
	Element *element = new ElementConst(value, set);
	Element *e = elemMap.get(element);
	if (e == NULL) {
		allSets.push(set);
                model_println("%%%%ElementConst:%u", allElements.getSize());
		allElements.push(element);
		elemMap.put(element, element);
		return element;
	} else {
		delete set;
		delete element;
		return e;
	}
}

Element *CSolver::applyFunction(Function *function, Element **array, uint numArrays, BooleanEdge overflowstatus) {
	Element *element = new ElementFunction(function,array,numArrays,overflowstatus);
	Element *e = elemMap.get(element);
	if (e == NULL) {
		element->updateParents();
                model_println("%%%%ElementFunction:%u", allElements.getSize());
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

BooleanEdge CSolver::getBooleanVar(VarType type) {
	Boolean *boolean = new BooleanVar(type);
	allBooleans.push(boolean);
	return BooleanEdge(boolean);
}

BooleanEdge CSolver::getBooleanTrue() {
	return boolTrue;
}

BooleanEdge CSolver::getBooleanFalse() {
	return boolFalse;
}

BooleanEdge CSolver::applyPredicate(Predicate *predicate, Element **inputs, uint numInputs) {
	return applyPredicateTable(predicate, inputs, numInputs, BooleanEdge(NULL));
}

BooleanEdge CSolver::applyPredicateTable(Predicate *predicate, Element **inputs, uint numInputs, BooleanEdge undefinedStatus) {
	BooleanPredicate *boolean = new BooleanPredicate(predicate, inputs, numInputs, undefinedStatus);
	Boolean *b = boolMap.get(boolean);
	if (b == NULL) {
		boolean->updateParents();
		boolMap.put(boolean, boolean);
		allBooleans.push(boolean);
		return BooleanEdge(boolean);
	} else {
		delete boolean;
		return BooleanEdge(b);
	}
}

bool CSolver::isTrue(BooleanEdge b) {
	return b.isNegated() ? b->isFalse() : b->isTrue();
}

bool CSolver::isFalse(BooleanEdge b) {
	return b.isNegated() ? b->isTrue() : b->isFalse();
}

BooleanEdge CSolver::applyLogicalOperation(LogicOp op, BooleanEdge arg1, BooleanEdge arg2) {
	BooleanEdge array[] = {arg1, arg2};
	return applyLogicalOperation(op, array, 2);
}

BooleanEdge CSolver::applyLogicalOperation(LogicOp op, BooleanEdge arg) {
	BooleanEdge array[] = {arg};
	return applyLogicalOperation(op, array, 1);
}

static int ptrcompares(const void *p1, const void *p2) {
	uintptr_t b1 = *(uintptr_t const *) p1;
	uintptr_t b2 = *(uintptr_t const *) p2;
	if (b1 < b2)
		return -1;
	else if (b1 == b2)
		return 0;
	else
		return 1;
}

BooleanEdge CSolver::rewriteLogicalOperation(LogicOp op, BooleanEdge *array, uint asize) {
	BooleanEdge newarray[asize];
	memcpy(newarray, array, asize * sizeof(BooleanEdge));
	for (uint i = 0; i < asize; i++) {
		BooleanEdge b = newarray[i];
		if (b->type == LOGICOP) {
			if (((BooleanLogic *) b.getBoolean())->replaced) {
				newarray[i] = doRewrite(newarray[i]);
				i--;//Check again
			}
		}
	}
	return applyLogicalOperation(op, newarray, asize);
}

BooleanEdge CSolver::applyLogicalOperation(LogicOp op, BooleanEdge *array, uint asize) {
	BooleanEdge newarray[asize];
	switch (op) {
	case SATC_NOT: {
		return array[0].negate();
	}
	case SATC_IFF: {
		for (uint i = 0; i < 2; i++) {
			if (array[i]->type == BOOLCONST) {
				if (isTrue(array[i])) {	// It can be undefined
					return array[1 - i];
				} else if (isFalse(array[i])) {
					newarray[0] = array[1 - i];
					return applyLogicalOperation(SATC_NOT, newarray, 1);
				}
			} else if (array[i]->type == LOGICOP) {
				BooleanLogic *b = (BooleanLogic *)array[i].getBoolean();
				if (b->replaced) {
					return rewriteLogicalOperation(op, array, asize);
				}
			}
		}
		break;
	}
	case SATC_OR: {
		for (uint i = 0; i < asize; i++) {
			newarray[i] = applyLogicalOperation(SATC_NOT, array[i]);
		}
		return applyLogicalOperation(SATC_NOT, applyLogicalOperation(SATC_AND, newarray, asize));
	}
	case SATC_AND: {
		uint newindex = 0;
		for (uint i = 0; i < asize; i++) {
			BooleanEdge b = array[i];
			if (b->type == LOGICOP) {
				if (((BooleanLogic *)b.getBoolean())->replaced)
					return rewriteLogicalOperation(op, array, asize);
			}
			if (b->type == BOOLCONST) {
				if (isTrue(b))
					continue;
				else {
					return boolFalse;
				}
			} else
				newarray[newindex++] = b;
		}
		if (newindex == 0) {
			return boolTrue;
		} else if (newindex == 1) {
			return newarray[0];
		} else {
			bsdqsort(newarray, newindex, sizeof(BooleanEdge), ptrcompares);
			array = newarray;
			asize = newindex;
		}
		break;
	}
	case SATC_XOR: {
		//handle by translation
		return applyLogicalOperation(SATC_NOT, applyLogicalOperation(SATC_IFF, array, asize));
	}
	case SATC_IMPLIES: {
		//handle by translation
		return applyLogicalOperation(SATC_OR, applyLogicalOperation(SATC_NOT, array[0]), array[1]);
	}
	}

	ASSERT(asize != 0);
	Boolean *boolean = new BooleanLogic(this, op, array, asize);
	Boolean *b = boolMap.get(boolean);
	if (b == NULL) {
		boolean->updateParents();
		boolMap.put(boolean, boolean);
		allBooleans.push(boolean);
		return BooleanEdge(boolean);
	} else {
		delete boolean;
		return BooleanEdge(b);
	}
}

BooleanEdge CSolver::orderConstraint(Order *order, uint64_t first, uint64_t second) {
	ASSERT(first != second);
	Boolean *constraint = new BooleanOrder(order, first, second);
	allBooleans.push(constraint);
	return BooleanEdge(constraint);
}

void CSolver::addConstraint(BooleanEdge constraint) {
#ifdef TRACE_DEBUG
        model_println("****New Constraint******");
#endif
	if (isTrue(constraint))
		return;
	else if (isFalse(constraint)) {
		int t = 0;
		setUnSAT();
	}
	else {
		if (constraint->type == LOGICOP) {
			BooleanLogic *b = (BooleanLogic *) constraint.getBoolean();
			if (!constraint.isNegated()) {
				if (b->op == SATC_AND) {
					for (uint i = 0; i < b->inputs.getSize(); i++) {
						addConstraint(b->inputs.get(i));
					}
					return;
				}
			}
			if (b->replaced) {
				addConstraint(doRewrite(constraint));
				return;
			}
		}
		constraints.add(constraint);
		Boolean *ptr = constraint.getBoolean();

		if (ptr->boolVal == BV_UNSAT) {
			setUnSAT();
		}

		replaceBooleanWithTrueNoRemove(constraint);
		constraint->parents.clear();
	}
}

Order *CSolver::createOrder(OrderType type, Set *set) {
	Order *order = new Order(type, set);
	allOrders.push(order);
	activeOrders.add(order);
	return order;
}

int CSolver::solve() {
	bool deleteTuner = false;
	if (tuner == NULL) {
		tuner = new DefaultTuner();
		deleteTuner = true;
	}

	long long startTime = getTimeNano();
	computePolarities(this);

	Preprocess pp(this);
	pp.doTransform();

	DecomposeOrderTransform dot(this);
	dot.doTransform();

	IntegerEncodingTransform iet(this);
	iet.doTransform();

	EncodingGraph eg(this);
	eg.buildGraph();
	eg.encode();
	printConstraints();
	naiveEncodingDecision(this);
	satEncoder->encodeAllSATEncoder(this);
	model_print("Is problem UNSAT after encoding: %d\n", unsat);
	int result = unsat ? IS_UNSAT : satEncoder->solve();
	model_print("Result Computed in CSolver: %d\n", result);
	long long finishTime = getTimeNano();
	elapsedTime = finishTime - startTime;
	if (deleteTuner) {
		delete tuner;
		tuner = NULL;
	}
	return result;
}

void CSolver::printConstraints() {
	SetIteratorBooleanEdge *it = getConstraints();
	while (it->hasNext()) {
		BooleanEdge b = it->next();
		if (b.isNegated())
			model_print("!");
		b->print();
		model_print("\n");
	}
	delete it;

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

bool CSolver::getBooleanValue(BooleanEdge bedge) {
	Boolean *boolean = bedge.getBoolean();
	switch (boolean->type) {
	case BOOLEANVAR:
		return getBooleanVariableValueSATTranslator(this, boolean);
	default:
		ASSERT(0);
	}
	exit(-1);
}

bool CSolver::getOrderConstraintValue(Order *order, uint64_t first, uint64_t second) {
	return order->encoding.resolver->resolveOrder(first, second);
}

long long CSolver::getEncodeTime() { return satEncoder->getEncodeTime(); }

long long CSolver::getSolveTime() { return satEncoder->getSolveTime(); }

void CSolver::autoTune(uint budget) {
	AutoTuner *autotuner = new AutoTuner(budget);
	autotuner->addProblem(this);
	autotuner->tune();
	delete autotuner;
}
