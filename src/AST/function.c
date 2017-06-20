#include "function.h"
#include "table.h"
#include "set.h"


Function* allocFunctionOperator( ArithOp op, Set ** domain, uint numDomain, Set * range,OverFlowBehavior overflowbehavior){
	FunctionOperator* This = (FunctionOperator*) ourmalloc(sizeof(FunctionOperator));
	GETFUNCTIONTYPE(This)=OPERATORFUNC;
	This->numDomains=numDomain;
	This->domains = ourmalloc(numDomain * sizeof(Set *));
	memcpy(This->domains, domain, numDomain * sizeof(Set *));
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

void deleteFunction(Function* This){
	switch(GETFUNCTIONTYPE(This)){
	case TABLEFUNC:
		break;
	case OPERATORFUNC:
		ourfree(((FunctionOperator*) This)->domains);
		break;
	default:
		ASSERT(0);
	}
	ourfree(This);
}
