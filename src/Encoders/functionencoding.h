#ifndef FUNCTIONENCODING_H
#define FUNCTIONENCODING_H
#include "classlist.h"

enum FunctionEncodingType {
	FUNC_UNASSIGNED, ENUMERATEIMPLICATIONS, ENUMERATEIMPLICATIONSNEGATE, CIRCUIT
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
void setFunctionEncodingType(FunctionEncoding* encoding, FunctionEncodingType type);
static inline FunctionEncodingType getFunctionEncodingType(FunctionEncoding* This) {return This->type;}
void deleteFunctionEncoding(FunctionEncoding *This);

#endif
