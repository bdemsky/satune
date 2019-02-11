#ifndef CSOLVER_H
#define CSOLVER_H
#include "classes.h"
#include "ops.h"
#include "corestructs.h"
#include "asthash.h"
#include "solver_interface.h"
#include "common.h"

class CSolver {
public:
	CSolver();
	~CSolver();
	void resetSolver();
	/** This function creates a set containing the elements passed in the array. */
	Set *createSet(VarType type, uint64_t *elements, uint num);

	/** This function creates a set from lowrange to highrange (inclusive). */

	Set *createRangeSet(VarType type, uint64_t lowrange, uint64_t highrange);

	bool itemExistInSet(Set *set, uint64_t item);

	VarType getSetVarType(Set *set);

	Element *createRangeVar(VarType type, uint64_t lowrange, uint64_t highrange);

	/** This function creates a mutable set.
	 * Note: You should use addItem for adding new item to Mutable sets, and
	 * at the end, you should call finalizeMutableSet!
	 */

	MutableSet *createMutableSet(VarType type);

	/** This function adds a new item to a set. */

	//Deprecating this unless we need it...
	void addItem(MutableSet *set, uint64_t element);

	/** This function adds a new unique item to the set and returns it.
	    This function cannot be used in conjunction with manually adding
	    items to the set. */

	uint64_t createUniqueItem(MutableSet *set);

	/**
	 * Freeze and finalize the mutableSet ...
	 */
	void finalizeMutableSet(MutableSet *set);

	/** This function creates an element variable over a set. */

	Element *getElementVar(Set *set);

	/** This function creates an element constrant. */
	Element *getElementConst(VarType type, uint64_t value);

	Set *getElementRange (Element *element);

	void mustHaveValue(Element *element);

	BooleanEdge getBooleanTrue();

	BooleanEdge getBooleanFalse();

	/** This function creates a boolean variable. */

	BooleanEdge getBooleanVar(VarType type);

	/** This function creates a function operator. */

	Function *createFunctionOperator(ArithOp op, Set *range,
																	 OverFlowBehavior overflowbehavior);

	/** This function creates a predicate operator. */

	Predicate *createPredicateOperator(CompOp op);

	Predicate *createPredicateTable(Table *table, UndefinedBehavior behavior);

	/** This function creates an empty instance table.*/

	Table *createTable(Set *range);

	Table *createTableForPredicate();
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

	void setUnSAT() { model_print("Setting UNSAT %%%%%%\n"); unsat = true; }
	void setSatSolverTimeout(long seconds) { satsolverTimeout = seconds;}
	bool isUnSAT() { return unsat; }
        bool isBooleanVarUsed(){return booleanVarUsed;}
	void printConstraint(BooleanEdge boolean);
	void printConstraints();

	Vector<Order *> *getOrders() { return &allOrders;}
	HashsetOrder *getActiveOrders() { return &activeOrders;}

	Tuner *getTuner() { return tuner; }

	SetIteratorBooleanEdge *getConstraints() { return constraints.iterator(); }

	SATEncoder *getSATEncoder() {return satEncoder;}

	void replaceBooleanWithTrue(BooleanEdge bexpr);
	void replaceBooleanWithTrueNoRemove(BooleanEdge bexpr);
	void replaceBooleanWithFalseNoRemove(BooleanEdge bexpr);
	void replaceBooleanWithFalse(BooleanEdge bexpr);
	void replaceBooleanWithBoolean(BooleanEdge oldb, BooleanEdge newb);
	CSolver *clone();
	void serialize();
	static CSolver *deserialize(const char *file, bool alloy = false);
	void autoTune(uint budget);
	void inferFixedOrders();
	void inferFixedOrder(Order *order);
	void setAlloyEncoder();
	bool useAlloyCompiler() {return alloyEncoder != NULL;}
	void setTuner(Tuner *_tuner) { tuner = _tuner; }
	long long getElapsedTime() { return elapsedTime; }
	long long getEncodeTime();
	long long getSolveTime();
	long getSatSolverTimeout() { return satsolverTimeout;}

	CMEMALLOC;

private:
	void handleIFFTrue(BooleanLogic *bexpr, BooleanEdge child);
	void handleANDTrue(BooleanLogic *bexpr, BooleanEdge child);
	void handleFunction(ElementFunction *ef, BooleanEdge child);

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
        bool booleanVarUsed;
        Tuner *tuner;
	long long elapsedTime;
	long satsolverTimeout;
	AlloyEnc *alloyEncoder;
	friend class ElementOpt;
	friend class VarOrderingOpt;
};

inline CompOp flipOp(CompOp op) {
	switch (op) {
	case SATC_EQUALS:
		return SATC_EQUALS;
	case SATC_LT:
		return SATC_GT;
	case SATC_GT:
		return SATC_LT;
	case SATC_LTE:
		return SATC_GTE;
	case SATC_GTE:
		return SATC_LTE;
	}
	ASSERT(0);
}

inline CompOp negateOp(CompOp op) {
	switch (op) {
	case SATC_EQUALS:
		ASSERT(0);
	case SATC_LT:
		return SATC_GTE;
	case SATC_GT:
		return SATC_LTE;
	case SATC_LTE:
		return SATC_GT;
	case SATC_GTE:
		return SATC_LT;
	}
	ASSERT(0);
}
#endif
