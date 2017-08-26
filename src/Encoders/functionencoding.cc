#include "functionencoding.h"

FunctionEncoding::FunctionEncoding(Element *function) :
	type(FUNC_UNASSIGNED),
	isFunction(true)
{
	op.function = function;
}

FunctionEncoding::FunctionEncoding(Boolean *predicate) :
	type(FUNC_UNASSIGNED),
	isFunction(false)
{
	op.predicate = predicate;
}

void FunctionEncoding::setFunctionEncodingType(FunctionEncodingType _type) {
	type = _type;
}
