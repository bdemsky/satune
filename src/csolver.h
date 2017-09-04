#ifndef CSOLVER_H
#define CSOLVER_H
#include "classes.h"
#include "ops.h"
#include "corestructs.h"
#include "asthash.h"
#include "solver_interface.h"

class CSolver {
public:
	CSolver();
	~CSolver();

	/** This function creates a set containing the elements passed in the array. */
	Set *createSet(VarType type, uint64_t *elements, uint num);

	/** This function creates a set from lowrange to highrange (inclusive). */

	Set *createRangeSet(VarType type, uint64_t lowrange, uint64_t highrange);

	Element *createRangeVar(VarType type, uint64_t lowrange, uint64_t highrange);

	/** This function creates a mutable set. */

	MutableSet *createMutableSet(VarType type);

	/** This function adds a new item to a set. */

	void addItem(MutableSet *set, uint64_t element);

	/** This function adds a new unique item to the set and returns it.
	    This function cannot be used in conjunction with manually adding
	    items to the set. */

	uint64_t createUniqueItem(MutableSet *set);

	/** This function creates an element variable over a set. */

	Element *getElementVar(Set *set);

	/** This function creates an element constrant. */
	Element *getElementConst(VarType type, uint64_t value);

	BooleanEdge getBooleanTrue();

	BooleanEdge getBooleanFalse();

	/** This function creates a boolean variable. */

	BooleanEdge getBooleanVar(VarType type);

	/** This function creates a function operator. */

	Function *createFunctionOperator(ArithOp op, Set **domain, uint numDomain, Set *range,
																	 OverFlowBehavior overflowbehavior);

	/** This function creates a predicate operator. */

	Predicate *createPredicateOperator(CompOp op, Set **domain, uint numDomain);

	Predicate *createPredicateTable(Table *table, UndefinedBehavior behavior);

	/** This function creates an empty instance table.*/

	Table *createTable(Set **domains, uint numDomain, Set *range);

	Table *createTableForPredicate(Set **domains, uint numDomain);
	/** This function adds an input output relation to a table. */

	void addTableEntry(Table *table, uint64_t *inputs, uint inputSize, uint64_t result);

	/** This function converts a completed table into a function. */

	Function *completeTable(Table *, UndefinedBehavior behavior);

	/** This function applies a function to the Elements in its input. */

	Element *applyFunction(Function *function, Element **array, uint numArrays, BooleanEdge overflowstatus);

	/** This function applies a predicate to the Elements in its input. */

	BooleanEdge applyPredicateTable(Predicate *predicate, Element **inputs, uint numInputs, BooleanEdge undefinedStatus);

	BooleanEdge applyPredicate(Predicate *predicate, Element **inputs, uint numInputs);

	/** This function applies a logical operation to the Booleans in its input. */

	BooleanEdge applyLogicalOperation(LogicOp op, BooleanEdge *array, uint asize);

	/** This function applies a logical operation to the Booleans in its input. */

	BooleanEdge applyLogicalOperation(LogicOp op, BooleanEdge arg1, BooleanEdge arg2);

	/** This function applies a logical operation to the Booleans in its input. */

	BooleanEdge applyLogicalOperation(LogicOp op, BooleanEdge arg);

	/** This function adds a boolean constraint to the set of constraints
	    to be satisfied */

	void addConstraint(BooleanEdge constraint);

	/** This function instantiates an order of type type over the set set. */
	Order *createOrder(OrderType type, Set *set);

	/** This function instantiates a boolean on two items in an order. */
	BooleanEdge orderConstraint(Order *order, uint64_t first, uint64_t second);

	/** When everything is done, the client calls this function and then csolver starts to encode*/
	int solve();

	/** After getting the solution from the SAT solver, client can get the value of an element via this function*/
	uint64_t getElementValue(Element *element);

	/** After getting the solution from the SAT solver, client can get the value of a boolean via this function*/
	bool getBooleanValue(BooleanEdge boolean);

	bool getOrderConstraintValue(Order *order, uint64_t first, uint64_t second);

	bool isTrue(BooleanEdge b);
	bool isFalse(BooleanEdge b);

	void setUnSAT() { unsat = true; }

	bool isUnSAT() { return unsat; }

	Vector<Order *> *getOrders() { return &allOrders;}
	HashsetOrder * getActiveOrders() { return &activeOrders;}

	Tuner *getTuner() { return tuner; }

	SetIteratorBooleanEdge * getConstraints() { return constraints.iterator(); }

	SATEncoder *getSATEncoder() {return satEncoder;}

	void replaceBooleanWithTrue(BooleanEdge bexpr);
	void replaceBooleanWithTrueNoRemove(BooleanEdge bexpr);
	void replaceBooleanWithFalse(BooleanEdge bexpr);
	void replaceBooleanWithBoolean(BooleanEdge oldb, BooleanEdge newb);
	CSolver *clone();
	void autoTune(uint budget);

	void setTuner(Tuner *_tuner) { tuner = _tuner; }
	long long getElapsedTime() { return elapsedTime; }
	long long getEncodeTime();
	long long getSolveTime();

	CMEMALLOC;

private:
	void handleIFFTrue(BooleanLogic *bexpr, BooleanEdge child);
	void handleANDTrue(BooleanLogic *bexpr, BooleanEdge child);

	//These two functions are helpers if the client has a pointer to a
	//Boolean object that we have since replaced
	BooleanEdge rewriteLogicalOperation(LogicOp op, BooleanEdge *array, uint asize);
	BooleanEdge doRewrite(BooleanEdge b);
	/** This is a vector of constraints that must be satisfied. */
	HashsetBooleanEdge constraints;

	/** This is a vector of all boolean structs that we have allocated. */
	Vector<Boolean *> allBooleans;

	/** This is a vector of all set structs that we have allocated. */
	Vector<Set *> allSets;

	/** This is a vector of all element structs that we have allocated. */
	Vector<Element *> allElements;

	/** This is a vector of all predicate structs that we have allocated. */
	Vector<Predicate *> allPredicates;

	/** This is a vector of all table structs that we have allocated. */
	Vector<Table *> allTables;

	/** This is a vector of all order structs that we have allocated. */
	Vector<Order *> allOrders;

	HashsetOrder activeOrders;
	
	/** This is a vector of all function structs that we have allocated. */
	Vector<Function *> allFunctions;

	BooleanEdge boolTrue;
	BooleanEdge boolFalse;

	/** These two tables are used for deduplicating entries. */
	BooleanMatchMap boolMap;
	ElementMatchMap elemMap;

	SATEncoder *satEncoder;
	bool unsat;
	Tuner *tuner;
	long long elapsedTime;
};
#endif
