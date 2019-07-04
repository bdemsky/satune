#ifndef __CCSOLVER_H
#define __CCSOLVER_H


typedef void *CCSolver;
#ifdef __cplusplus
extern "C" {
#endif
void *createCCSolver();
void deleteCCSolver(void *solver);
void resetCCSolver(void *solver);
void *createSet(void *solver,unsigned int type, long *elements, unsigned int num);
void *createRangeSet(void *solver,unsigned int type, long lowrange, long highrange);
void *createRangeVar(void *solver,unsigned int type, long lowrange, long highrange);
void *createMutableSet(void *solver,unsigned int type);
void addItem(void *solver,void *set, long element);
void finalizeMutableSet(void *solver,void *set);
void *getElementVar(void *solver,void *set);
void *getElementConst(void *solver,unsigned int type, long value);
void *getElementRange (void *solver,void *element);
void *getBooleanVar(void *solver,unsigned int type);
void *getBooleanTrue(void *solver);
void *getBooleanFalse(void *solver);
void *createFunctionOperator(void *solver,unsigned int op, void *range,unsigned int overflowbehavior);
void *createPredicateOperator(void *solver,unsigned int op);
void *createPredicateTable(void *solver,void *table, unsigned int behavior);
void *createTable(void *solver, void *range);
void *createTableForPredicate(void *solver);
void addTableEntry(void *solver,void *table, void *inputs, unsigned int inputSize, long result);
void *completeTable(void *solver,void *table, unsigned int behavior);
void *applyFunction(void *solver,void *function, void **array, unsigned int numArrays, void *overflowstatus);
void *applyPredicateTable(void *solver,void *predicate, void **inputs, unsigned int numInputs, void *undefinedStatus);
void *applyPredicate(void *solver,void *predicate, void **inputs, unsigned int numInputs);
void *applyLogicalOperation(void *solver,unsigned int op, void **array, unsigned int asize);
void *applyExactlyOneConstraint(void *solver,void **array, unsigned int asize);
void *applyLogicalOperationTwo(void *solver,unsigned int op, void *arg1, void *arg2);
void *applyLogicalOperationOne(void *solver,unsigned int op, void *arg);
void addConstraint(void *solver,void *constraint);
void printConstraint(void *solver,void *constraint);
void *createOrder(void *solver,unsigned int type, void *set);
void *orderConstraint(void *solver,void *order, long first, long second);
int solve(void *solver);
int solveIncremental(void *solver);
long getElementValue(void *solver,void *element);
void freezeElement(void *solver,void *element);
int getBooleanValue(void *solver,void *boolean);
int getOrderConstraintValue(void *solver,void *order, long first, long second);
void printConstraints(void *solver);
void turnoffOptimizations(void *solver);
void serialize(void *solver);
void mustHaveValue(void *solver, void *element);
void setInterpreter(void *solver, unsigned int type);
void *clone(void *solver);
#ifdef __cplusplus
}
#endif

#endif
