#ifndef FUNCTIONENCODING_H
#define FUNCTIONENCODING_H
#include "classlist.h"

enum FunctionEncodingType {
	ENUMERATEIMPLICATIONS, CIRCUIT
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

FunctionEncoding * allocFunctionEncoding(FunctionEncodingType type, Element *function);
FunctionEncoding * allocPredicateEncoding(FunctionEncodingType type, Boolean *predicate);
void deleteFunctionEncoding(FunctionEncoding *This);
#endif
