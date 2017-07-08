#include "function.h"
#include "table.h"
#include "set.h"


Function* allocFunctionOperator( ArithOp op, Set ** domain, uint numDomain, Set * range,OverFlowBehavior overflowbehavior){
	FunctionOperator* This = (FunctionOperator*) ourmalloc(sizeof(FunctionOperator));
	GETFUNCTIONTYPE(This)=OPERATORFUNC;
	allocInlineArrayInitSet(&This->domains, domain, numDomain);
	This->op=op;
	This->overflowbehavior = overflowbehavior;
	This->range=range;
	return &This->base;
}

Function* allocFunctionTable (Table* table){
	FunctionTable* This = (FunctionTable*) ourmalloc(sizeof(FunctionTable));
	GETFUNCTIONTYPE(This)=TABLEFUNC;
	This->table = table;
	return &This->base;
}

uint64_t applyFunctionOperator(FunctionOperator* func, uint64_t var1, uint64_t var2, bool* isInRange){
	uint64_t result = 0;
	switch( func->op){
		case ADD:
			result = var1+ var2;
			break;
		case SUB:
			result = var1 - var2;
			break;
		default:
			ASSERT(0);
	}
	*isInRange = existsInSet(func->range, result);
	return result;
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
