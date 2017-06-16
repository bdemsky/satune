#include "functionencoding.h"

FunctionEncoding * allocFunctionEncoding(FunctionEncodingType type, Element *function) {
	FunctionEncoding * this=(FunctionEncoding *)ourmalloc(sizeof(FunctionEncoding));
	this->op.function=function;
	this->type=type;
	return this;
}

FunctionEncoding * allocPredicateEncoding(FunctionEncodingType type, Boolean *predicate) {
	FunctionEncoding * this=(FunctionEncoding *)ourmalloc(sizeof(FunctionEncoding));
	this->op.predicate=predicate;
	this->type=type;
	return this;
}

void deleteFunctionEncoding(FunctionEncoding *this) {
	ourfree(this);
}
