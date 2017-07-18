#include "function.h"
#include "table.h"
#include "set.h"


Function* allocFunctionOperator(ArithOp op, Set ** domain, uint numDomain, Set * range, OverFlowBehavior overflowbehavior) {
	FunctionOperator* This = (FunctionOperator*) ourmalloc(sizeof(FunctionOperator));
	GETFUNCTIONTYPE(This)=OPERATORFUNC;
	initArrayInitSet(&This->domains, domain, numDomain);
	This->op=op;
	This->overflowbehavior = overflowbehavior;
	This->range=range;
	return &This->base;
}

Function* allocFunctionTable (Table* table, UndefinedBehavior undefBehavior){
	FunctionTable* This = (FunctionTable*) ourmalloc(sizeof(FunctionTable));
	GETFUNCTIONTYPE(This)=TABLEFUNC;
	This->table = table;
	This->undefBehavior = undefBehavior;
	return &This->base;
}

uint64_t applyFunctionOperator(FunctionOperator* This, uint numVals, uint64_t * values) {
	ASSERT(numVals == 2);
	switch(This->op){
		case ADD:
			return values[0] + values[1];
			break;
		case SUB:
			return values[0] - values[1];
			break;
		default:
			ASSERT(0);
	}
}

bool isInRangeFunction(FunctionOperator *This, uint64_t val) {
	return existsInSet(This->range, val);
}

void deleteFunction(Function* This){
	switch(GETFUNCTIONTYPE(This)){
	case TABLEFUNC:
		break;
	case OPERATORFUNC:
		deleteInlineArraySet(&((FunctionOperator*) This)->domains);
		break;
	default:
		ASSERT(0);
	}
	ourfree(This);
}
