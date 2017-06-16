#include "functionencoder.h"

FunctionEncoder * allocFunctionEncoder(FunctionEncoderType type, Function *function) {
	FunctionEncoder * this=(FunctionEncoder *)ourmalloc(sizeof(FunctionEncoder));
	this->function=function;
	this->type=type;
	return this;
}

void deleteFunctionEncoder(FunctionEncoder *this) {
	ourfree(this);
}
