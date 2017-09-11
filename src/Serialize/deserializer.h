
/* 
 * File:   deserializer.h
 * Author: hamed
 *
 * Created on September 7, 2017, 6:07 PM
 */

#ifndef DESERIALIZER_H
#define DESERIALIZER_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
/**
 * Style of serialized file:
 * ASTNodeType#Pointer#ObjectDATA
 * 
 * @param file
 */
class Deserializer {
public:
	Deserializer(const char* file);
	CSolver *deserialize();
	virtual ~Deserializer();
private:
	ssize_t myread (void *__buf, size_t __nbytes);
	void deserializeBooleanEdge();
	void deserializeBooleanVar();
	void deserializeBooleanOrder();
	void deserializeOrder();
	void deserializeSet();
	void deserializeBooleanLogic();
	void deserializeBooleanPredicate();
	void deserializePredicateTable();
	void deserializePredicateOperator();
	void deserializeTable();
	void deserializeElementSet();
	void deserializeElementConst();
	void deserializeElementFunction();
	void deserializeFunctionOperator();
	void deserializeFunctionTable();
	CSolver *solver;
	int filedesc;
	CloneMap map;
};

#endif /* DESERIALIZER_H */

