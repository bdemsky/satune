#include "function.h"
#include "table.h"
#include "set.h"


Function* allocFunctionOperator( ArithOp op, Set ** domain, uint numDomain, Set * range,OverFlowBehavior overflowbehavior){
    FunctionOperator* This = (FunctionOperator*) ourmalloc(sizeof(FunctionOperator));
    GETFUNCTIONTYPE(This)=OPERATORFUNC;
    This->domains = allocVectorArraySet(numDomain, domain);
    This->op=op;
    This->overflowbehavior = overflowbehavior;
    This->range=range;
    return  &This->base;
}

Function* allocFunctionTable (Table* table){
    FunctionTable* This = (FunctionTable*) ourmalloc(sizeof(FunctionTable));
    GETFUNCTIONTYPE(This)=TABLEFUNC;
    This->table = table;
}

void deleteFunction(Function* This){
    switch( GETFUNCTIONTYPE(This)){
	case TABLEFUNC:
	    ourfree((FunctionTable*)This);
	    break;
	case OPERATORFUNC:
	    ourfree((FunctionOperator*) This);
	    break;
	default:
	    ASSERT(0);
    }
}
