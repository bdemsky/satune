#ifndef FUNCTIONENCODER_H
#define FUNCTIONENCODER_H
#include "classlist.h"

enum FunctionEncoderType {
	ENUMERATEIMPLICATIONS, CIRCUIT
};

typedef enum FunctionEncoderType FunctionEncoderType;

union ElementPredicate {
	Element * function;
	Boolean * predicate;
};

typedef union ElementPredicate ElementPredicate;

struct FunctionEncoder {
	FunctionEncoderType type;
	bool isFunction; //true for function, false for predicate
	ElementPredicate op;
};

FunctionEncoder * allocFunctionEncoder(FunctionEncoderType type, Element *function);
FunctionEncoder * allocPredicateEncoder(FunctionEncoderType type, Boolean *predicate);
void deleteFunctionEncoder(FunctionEncoder *this);
#endif
