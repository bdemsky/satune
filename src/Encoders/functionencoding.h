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

void initFunctionEncoding(FunctionEncoding *encoding, Element *function);
void initPredicateEncoding(FunctionEncoding *encoding, Boolean *predicate);
void deleteFunctionEncoding(FunctionEncoding *This);

#endif
