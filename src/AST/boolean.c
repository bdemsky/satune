#include "boolean.h"
#include "structs.h"
#include "csolver.h"
#include "element.h"
#include "order.h"

Boolean* allocBooleanVar(VarType t) {
	BooleanVar* This=(BooleanVar *) ourmalloc(sizeof (BooleanVar));
	GETBOOLEANTYPE(This)=BOOLEANVAR;
	This->vtype=t;
	This->var=E_NULL;
	initDefVectorBoolean(GETBOOLEANPARENTS(This));
	return & This->base;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	BooleanOrder* This=(BooleanOrder *) ourmalloc(sizeof (BooleanOrder));
	GETBOOLEANTYPE(This)=ORDERCONST;
	This->order=order;
	This->first=first;
	This->second=second;
	pushVectorBoolean(&order->constraints, &This->base);
	initDefVectorBoolean(GETBOOLEANPARENTS(This));
	return & This -> base;
}

Boolean * allocBooleanPredicate(Predicate * predicate, Element ** inputs, uint numInputs, Boolean* undefinedStatus){
	BooleanPredicate* This = (BooleanPredicate*) ourmalloc(sizeof(BooleanPredicate));
	GETBOOLEANTYPE(This)= PREDICATEOP;
	This->predicate=predicate;
	initArrayInitElement(&This->inputs, inputs, numInputs);
	initDefVectorBoolean(GETBOOLEANPARENTS(This));

	for(uint i=0;i<numInputs;i++) {
		pushVectorASTNode(GETELEMENTPARENTS(inputs[i]), (ASTNode *)This);
	}
	initPredicateEncoding(&This->encoding, (Boolean *) This);
	This->undefStatus = undefinedStatus;
	return & This->base;
}

Boolean * allocBooleanLogicArray(CSolver *solver, LogicOp op, Boolean ** array, uint asize){
	BooleanLogic * This = ourmalloc(sizeof(BooleanLogic));
	initDefVectorBoolean(GETBOOLEANPARENTS(This));
	initArrayInitBoolean(&This->inputs, array, asize);
	pushVectorBoolean(solver->allBooleans, (Boolean *) This);
	return & This->base;
}

void deleteBoolean(Boolean * This) {
	switch(GETBOOLEANTYPE(This)){
	case PREDICATEOP: {
		BooleanPredicate *bp=(BooleanPredicate *)This;
		deleteInlineArrayElement(& bp->inputs );
		deleteFunctionEncoding(& bp->encoding);
		break;
	}
	default:
		break;
	}
	deleteVectorArrayBoolean(GETBOOLEANPARENTS(This));
	ourfree(This);
}
