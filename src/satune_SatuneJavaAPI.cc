/* DO NOT EDIT THIS FILE - it is machine generated */
#include "satune_SatuneJavaAPI.h"
#include "ccsolver.h"
#define CCSOLVER(solver) (void*)solver
/* Header for class SatuneJavaAPI */

/*
 * Class:     SatuneJavaAPI
 * Method:    createCCSolver
 * Signature: ()J
 */

JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createCCSolver
	(JNIEnv *env, jobject obj)
{
	return (jlong)createCCSolver();
	
}

/*
 * Class:     SatuneJavaAPI
 * Method:    deleteCCSolver
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_deleteCCSolver
	(JNIEnv *env, jobject obj, jlong solver)
{
	deleteCCSolver(CCSOLVER(solver));
}

/*
 * Class:     SatuneJavaAPI
 * Method:    resetCCSolver
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_resetCCSolver
	(JNIEnv *env, jobject obj, jlong solver)
{
	resetCCSolver(CCSOLVER(solver));
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createSet
 * Signature: (JIJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createSet
	(JNIEnv *env, jobject obj, jlong solver, jint type, jlong elements, jint num)
{
	return (jlong)createSet((void *)solver,(unsigned int) type, (long *)elements, (unsigned int) num);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createRangeSet
 * Signature: (JIJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createRangeSet
	(JNIEnv *env, jobject obj, jlong solver, jint type, jlong lowrange, jlong highrange)
{
	return (jlong)createRangeSet((void *)solver,(unsigned int) type, (long) lowrange, (long) highrange);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createRangeVar
 * Signature: (JIJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createRangeVar
	(JNIEnv *env, jobject obj, jlong solver, jint type, jlong lowrange, jlong highrange)
{
	return (jlong)createRangeVar((void *)solver,(unsigned int) type, (long) lowrange, (long) highrange);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createMutableSet
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createMutableSet
	(JNIEnv *env, jobject obj, jlong solver, jint type)
{
	return (jlong)createMutableSet((void *)solver,(unsigned int) type);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    addItem
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_addItem
	(JNIEnv *env, jobject obj, jlong solver, jlong set, jlong element)
{
	addItem((void *)solver,(void *)set, (long) element);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    finalizeMutableSet
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_finalizeMutableSet
	(JNIEnv *ev, jobject obj, jlong solver, jlong set)
{
	finalizeMutableSet((void *)solver,(void *)set);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getElementVar
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementVar
	(JNIEnv *env, jobject obj, jlong solver, jlong set)
{
	return (jlong) getElementVar((void *)solver,(void *)set);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getElementConst
 * Signature: (JIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementConst
	(JNIEnv *env, jobject obj, jlong solver, jint type, jlong value)
{
	return (jlong)getElementConst((void *)solver,(unsigned int) type, (long) value);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getElementRange
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementRange
	(JNIEnv *env, jobject obj, jlong solver, jlong element)
{
	return (jlong)getElementRange ((void *)solver,(void *)element);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getBooleanVar
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getBooleanVar
	(JNIEnv *env, jobject obj, jlong solver, jint type)
{
	return (jlong)getBooleanVar((void *)solver,(unsigned int) type);
}


/*
 * Class:     SatuneJavaAPI
 * Method:    getBooleanVar
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getBooleanTrue
	(JNIEnv * env, jobject obj, jlong solver)
{
	return (jlong)getBooleanTrue((void *)solver);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getBooleanVar
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getBooleanFalse
	(JNIEnv * env, jobject obj, jlong solver)
{
	return (jlong)getBooleanFalse((void *)solver);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createFunctionOperator
 * Signature: (JIJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createFunctionOperator
	(JNIEnv *env, jobject obj, jlong solver, jint op, jlong range, jint overflowbehavior)
{
	return (jlong)createFunctionOperator((void *)solver,(unsigned int) op, (void *)range,(unsigned int) overflowbehavior);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createPredicateOperator
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createPredicateOperator
	(JNIEnv *env, jobject obj, jlong solver, jint op)
{
	return (jlong)createPredicateOperator((void *)solver,(unsigned int) op);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createPredicateTable
 * Signature: (JJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createPredicateTable
	(JNIEnv *env, jobject obj, jlong solver, jlong table, jint behavior)
{
	return (jlong) createPredicateTable((void *)solver,(void *)table, (unsigned int) behavior);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createTable
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createTable
	(JNIEnv *env, jobject obj, jlong solver, jlong range)
{
	return (jlong)createTable((void *)solver, (void *)range);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createTableForPredicate
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createTableForPredicate
	(JNIEnv *env, jobject obj, jlong solver)
{
	return (jlong)createTableForPredicate((void *)solver);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    addTableEntry
 * Signature: (JJJIJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_addTableEntry
	(JNIEnv *env, jobject obj, jlong solver, jlong table, jlong inputs, jint inputSize, jlong result)
{
	addTableEntry((void *)solver,(void *)table, (void *)inputs, (unsigned int) inputSize, (long) result);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    completeTable
 * Signature: (JJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_completeTable
	(JNIEnv *env, jobject obj, jlong solver, jlong table, jint behavior)
{
	return (jlong)completeTable((void *)solver,(void *)table, (unsigned int) behavior);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    applyFunction
 * Signature: (JJJIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyFunction
	(JNIEnv *env, jobject obj, jlong solver, jlong function, jlong array, jint numArrays, jlong overflowstatus)
{
	return (jlong)applyFunction((void *)solver,(void *)function, (void **)array, (unsigned int) numArrays, (void *)overflowstatus);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    applyPredicateTable
 * Signature: (JJJIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyPredicateTable
	(JNIEnv *env, jobject obj, jlong solver, jlong predicate, jlong inputs, jint numInputs, jlong undefinedStatus)
{
	return (jlong) applyPredicateTable((void *)solver,(void *)predicate, (void **)inputs, (unsigned int) numInputs, (void *)undefinedStatus);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    applyPredicate
 * Signature: (JJJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyPredicate
	(JNIEnv *env, jobject obj, jlong solver, jlong predicate, jlong inputs, jint numInputs)
{
	return (jlong)applyPredicate((void *)solver,(void *)predicate, (void **)inputs, (unsigned int) numInputs);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    applyLogicalOperation
 * Signature: (JIJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyLogicalOperation
	(JNIEnv *env, jobject obj, jlong solver, jint op, jlong array, jint asize)
{
	return (jlong)applyLogicalOperation((void *)solver,(unsigned int) op, (void *)array, (unsigned int) asize);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    applyLogicalOperationTwo
 * Signature: (JIJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyLogicalOperationTwo
	(JNIEnv *env, jobject obj, jlong solver, jint op, jlong arg1, jlong arg2)
{
	return (jlong) applyLogicalOperationTwo((void *)solver,(unsigned int) op, (void *)arg1, (void *)arg2);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    applyLogicalOperationOne
 * Signature: (JIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyLogicalOperationOne
	(JNIEnv *env, jobject obj, jlong solver, jint op, jlong arg)
{
	return (jlong)applyLogicalOperationOne((void *)solver,(unsigned int) op, (void *)arg);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    addConstraint
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_addConstraint
	(JNIEnv *env, jobject obj, jlong solver, jlong constraint)
{
	addConstraint((void *)solver,(void *)constraint);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    printConstraint
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_printConstraint
	(JNIEnv *env, jobject obj, jlong solver, jlong constraint)
{
	printConstraint((void *)solver,(void *)constraint);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    createOrder
 * Signature: (JIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createOrder
	(JNIEnv *env, jobject obj, jlong solver, jint type, jlong set)
{
	return (jlong)createOrder((void *)solver,(unsigned int) type, (void *)set);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    orderConstraint
 * Signature: (JJJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_orderConstraint
	(JNIEnv *env, jobject obj, jlong solver, jlong order, jlong first, jlong second)
{
	return (jlong)orderConstraint((void *)solver,(void *)order, (long) first, (long) second);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    solve
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_satune_SatuneJavaAPI_solve
	(JNIEnv *env, jobject obj, jlong solver)
{
	return (jint) solve((void *)solver);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getElementValue
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementValue
	(JNIEnv *env, jobject obj, jlong solver, jlong element)
{
	return (jlong) getElementValue((void *)solver,(void *)element);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getBooleanValue
 * Signature: (JJ)I
 */
JNIEXPORT jint JNICALL Java_satune_SatuneJavaAPI_getBooleanValue
	(JNIEnv *env, jobject obj, jlong solver, jlong boolean)
{
	return (jint) getBooleanValue((void *)solver, (void *)boolean);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    getOrderConstraintValue
 * Signature: (JJJJ)I
 */
JNIEXPORT jint JNICALL Java_satune_SatuneJavaAPI_getOrderConstraintValue
	(JNIEnv *env, jobject obj, jlong solver, jlong order, jlong first, jlong second)
{
	return (jint) getOrderConstraintValue((void *)solver,(void *)order, (long) first, (long) second);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    printConstraints
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_printConstraints
	(JNIEnv *env, jobject obj, jlong solver)
{
	printConstraints((void *)solver);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    serialize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_serialize
	(JNIEnv *env, jobject obj, jlong solver)
{
	serialize((void *)solver);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    mustHaveValue
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_mustHaveValue
	(JNIEnv *env, jobject obj, jlong solver, jlong element)
{
	mustHaveValue((void *)solver, (void *)element);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    setInterpreter
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_setInterpreter
	(JNIEnv *env, jobject obj, jlong solver, jint type)
{
	setInterpreter((void *)solver, (unsigned int) type);
}

/*
 * Class:     SatuneJavaAPI
 * Method:    clone
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_clone
	(JNIEnv *env, jobject obj, jlong solver)
{
	return (jlong)clone((void *)solver);
}
