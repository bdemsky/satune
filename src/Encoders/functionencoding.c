#include "functionencoding.h"

FunctionEncoding * allocFunctionEncoding(FunctionEncodingType type, Element *function) {
	FunctionEncoding * This=(FunctionEncoding *)ourmalloc(sizeof(FunctionEncoding));
	This->op.function=function;
	This->type=type;
	return This;
}

FunctionEncoding * allocPredicateEncoding(FunctionEncodingType type, Boolean *predicate) {
	FunctionEncoding * This=(FunctionEncoding *)ourmalloc(sizeof(FunctionEncoding));
	This->op.predicate=predicate;
	This->type=type;
	return This;
}

void deleteFunctionEncoding(FunctionEncoding *This) {
	ourfree(This);
}
