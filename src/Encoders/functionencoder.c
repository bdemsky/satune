#include "functionencoder.h"

FunctionEncoder * allocFunctionEncoder(FunctionEncoderType type, Element *function) {
	FunctionEncoder * this=(FunctionEncoder *)ourmalloc(sizeof(FunctionEncoder));
	this->op.function=function;
	this->type=type;
	return this;
}

FunctionEncoder * allocPredicateEncoder(FunctionEncoderType type, Boolean *predicate) {
	FunctionEncoder * this=(FunctionEncoder *)ourmalloc(sizeof(FunctionEncoder));
	this->op.predicate=predicate;
	this->type=type;
	return this;
}

void deleteFunctionEncoder(FunctionEncoder *this) {
	ourfree(this);
}
