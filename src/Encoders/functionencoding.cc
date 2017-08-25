#include "functionencoding.h"

FunctionEncoding::FunctionEncoding(Element *function) :
	type(FUNC_UNASSIGNED)
{
	op.function = function;
}

FunctionEncoding::FunctionEncoding(Boolean *predicate) :
	type(FUNC_UNASSIGNED)
{
	op.predicate = predicate;
}

void FunctionEncoding::setFunctionEncodingType(FunctionEncodingType _type) {
	type = _type;
}
