#ifndef FUNCTIONENCODING_H
#define FUNCTIONENCODING_H
#include "classlist.h"

enum FunctionEncodingType {
	FUNC_UNASSIGNED, ENUMERATEIMPLICATIONS, CIRCUIT
};

typedef enum FunctionEncodingType FunctionEncodingType;

union ElementPredicate {
	Element * function;
	Boolean * predicate;
};

typedef union ElementPredicate ElementPredicate;

struct FunctionEncoding {
	FunctionEncodingType type;
	bool isFunction;//true for function, false for predicate
	ElementPredicate op;
};

inline FunctionEncoding* getFunctionEncoding(ASTNode func){
	switch(GETASTNODETYPE(func)){
		case ELEMFUNCRETURN:
			return &((ElementFunction*)func)->functionencoding;
		case PREDICATEOP:
			return &((BooleanPredicate*)func)->encoding;
		default:
			ASSERT(0);
	}
	return NULL;
}

void initFunctionEncoding(FunctionEncoding *encoding, Element *function);
void initPredicateEncoding(FunctionEncoding *encoding, Boolean *predicate);
void setFunctionEncodingType(FunctionEncoding* encoding, FunctionEncodingType type);
void deleteFunctionEncoding(FunctionEncoding *This);

#endif
