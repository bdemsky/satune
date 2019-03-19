/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class satune_SatuneJavaAPI */

#ifndef _Included_satune_SatuneJavaAPI
#define _Included_satune_SatuneJavaAPI
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createCCSolver
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createCCSolver
	(JNIEnv *, jobject);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    deleteCCSolver
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_deleteCCSolver
	(JNIEnv *, jobject, jlong);


/*
 * Class:     satune_SatuneJavaAPI
 * Method:    resetCCSolver
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_resetCCSolver
	(JNIEnv *, jobject, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createSet
 * Signature: (JIJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createSet
	(JNIEnv *, jobject, jlong, jint, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createRangeSet
 * Signature: (JIJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createRangeSet
	(JNIEnv *, jobject, jlong, jint, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createRangeVar
 * Signature: (JIJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createRangeVar
	(JNIEnv *, jobject, jlong, jint, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createMutableSet
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createMutableSet
	(JNIEnv *, jobject, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    addItem
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_addItem
	(JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    finalizeMutableSet
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_finalizeMutableSet
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getElementVar
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementVar
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getElementConst
 * Signature: (JIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementConst
	(JNIEnv *, jobject, jlong, jint, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getElementRange
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementRange
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getBooleanVar
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getBooleanVar
	(JNIEnv *, jobject, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getBooleanTrue
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getBooleanTrue
	(JNIEnv *, jobject, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getBooleanFalse
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getBooleanFalse
	(JNIEnv *, jobject, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createFunctionOperator
 * Signature: (JIJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createFunctionOperator
	(JNIEnv *, jobject, jlong, jint, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createPredicateOperator
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createPredicateOperator
	(JNIEnv *, jobject, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createPredicateTable
 * Signature: (JJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createPredicateTable
	(JNIEnv *, jobject, jlong, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createTable
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createTable
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createTableForPredicate
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createTableForPredicate
	(JNIEnv *, jobject, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    addTableEntry
 * Signature: (JJJIJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_addTableEntry
	(JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    completeTable
 * Signature: (JJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_completeTable
	(JNIEnv *, jobject, jlong, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    applyFunction
 * Signature: (JJJIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyFunction
	(JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    applyPredicateTable
 * Signature: (JJJIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyPredicateTable
	(JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    applyPredicate
 * Signature: (JJJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyPredicate
	(JNIEnv *, jobject, jlong, jlong, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    applyLogicalOperation
 * Signature: (JIJI)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyLogicalOperation
	(JNIEnv *, jobject, jlong, jint, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    applyLogicalOperationTwo
 * Signature: (JIJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyLogicalOperationTwo
	(JNIEnv *, jobject, jlong, jint, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    applyLogicalOperationOne
 * Signature: (JIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_applyLogicalOperationOne
	(JNIEnv *, jobject, jlong, jint, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    addConstraint
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_addConstraint
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    printConstraint
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_printConstraint
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    createOrder
 * Signature: (JIJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_createOrder
	(JNIEnv *, jobject, jlong, jint, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    orderConstraint
 * Signature: (JJJJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_orderConstraint
	(JNIEnv *, jobject, jlong, jlong, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    solve
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_satune_SatuneJavaAPI_solve
	(JNIEnv *, jobject, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getElementValue
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_getElementValue
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getBooleanValue
 * Signature: (JJ)I
 */
JNIEXPORT jint JNICALL Java_satune_SatuneJavaAPI_getBooleanValue
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    getOrderConstraintValue
 * Signature: (JJJJ)I
 */
JNIEXPORT jint JNICALL Java_satune_SatuneJavaAPI_getOrderConstraintValue
	(JNIEnv *, jobject, jlong, jlong, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    printConstraints
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_printConstraints
	(JNIEnv *, jobject, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    serialize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_serialize
	(JNIEnv *, jobject, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    mustHaveValue
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_mustHaveValue
	(JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    setInterpreter
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_satune_SatuneJavaAPI_setInterpreter
	(JNIEnv *, jobject, jlong, jint);

/*
 * Class:     satune_SatuneJavaAPI
 * Method:    clone
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_satune_SatuneJavaAPI_clone
	(JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif