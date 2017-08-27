#ifndef FUNCTIONENCODING_H
#define FUNCTIONENCODING_H
#include "classlist.h"

enum FunctionEncodingType {
	FUNC_UNASSIGNED, ENUMERATEIMPLICATIONS, ENUMERATEIMPLICATIONSNEGATE, CIRCUIT
};

typedef enum FunctionEncodingType FunctionEncodingType;

union ElementPredicate {
	Element *function;
	Boolean *predicate;
};

typedef union ElementPredicate ElementPredicate;

class FunctionEncoding {
public:
	FunctionEncodingType type;
	bool isFunction;//true for function, false for predicate
	ElementPredicate op;
	FunctionEncoding(Element *function);
	FunctionEncoding(Boolean *predicate);
	void setFunctionEncodingType(FunctionEncodingType type);
	FunctionEncodingType getFunctionEncodingType() {return type;}
	MEMALLOC;
};



#endif
