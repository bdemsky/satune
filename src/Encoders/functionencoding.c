#include "functionencoding.h"

void initFunctionEncoding(FunctionEncoding *This, Element *function) {
	This->op.function=function;
	This->type=FUNC_UNASSIGNED;
}

void initPredicateEncoding(FunctionEncoding *This,  Boolean *predicate) {
	This->op.predicate=predicate;
	This->type=FUNC_UNASSIGNED;
}

void deleteFunctionEncoding(FunctionEncoding *This) {
}
