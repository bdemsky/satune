#ifndef __CCSOLVER_H
#define __CCSOLVER_H


typedef void* CCSolver;
#ifdef __cplusplus
extern "C" {
#endif
void* CreateCCSolver();
void deleteCCSolver(void* solver);
void *createSet(void* solver,unsigned int type, void *elements, unsigned int num);
void *createRangeSet(void* solver,unsigned int type, long lowrange, long highrange);
void *createRangeVar(void* solver,unsigned type, long lowrange, long highrange);
void *createMutableSet(void* solver,unsigned type);
void addItem(void* solver,void *set, long element);
void finalizeMutableSet(void* solver,void *set);
void *getElementVar(void* solver,void *set);
void *getElementConst(void* solver,unsigned type, long value);
void *getElementRange (void* solver,void *element);
void* getBooleanVar(void* solver,unsigned int type);
void *createFunctionOperator(void* solver,unsigned int op, void *domain, unsigned int numDomain, void *range,unsigned int overflowbehavior);
void *createPredicateOperator(void* solver,unsigned int op, void *domain, unsigned int numDomain);
void *createPredicateTable(void* solver,void *table, unsigned int behavior);
void *createTable(void* solver,void*domains, unsigned int numDomain, void *range);
void *createTableForPredicate(void* solver,void*domains, unsigned int numDomain);
void addTableEntry(void* solver,void *table, void *inputs, unsigned int inputSize, long result);
void *completeTable(void* solver,void *table, unsigned int behavior);
void *applyFunction(void* solver,void *function, void*array, unsigned int numArrays, long overflowstatus);
void* applyPredicateTable(void* solver,void *predicate, void *inputs, unsigned int numInputs, long undefinedStatus);
void* applyPredicate(void* solver,void *predicate, void *inputs, unsigned int numInputs);
void* applyLogicalOperation(void* solver,unsigned int op, void *array, unsigned int asize);
void* applyLogicalOperationTwo(void* solver,unsigned int op, long arg1, long arg2);
void* applyLogicalOperationOne(void* solver,unsigned int op, long arg);
void addConstraint(void* solver,void* constraint);
void *createOrder(void* solver,unsigned int type, void *set);
void* orderConstraint(void* solver,void *order, long first, long second);
int solve(void* solver);
long getElementValue(void* solver,void *element);
int getBooleanValue(void* solver,long boolean);
int getOrderConstraintValue(void* solver,void *order, long first, long second);
void printConstraints(void* solver);
#ifdef __cplusplus
}
#endif

#endif
