#include "boolean.h"
#include "structs.h"
#include "csolver.h"
#include "element.h"
#include "order.h"

Boolean* allocBoolean(VarType t) {
	BooleanVar* tmp=(BooleanVar *) ourmalloc(sizeof (BooleanVar));
	GETBOOLEANTYPE(tmp)=BOOLEANVAR;
	tmp->vtype=t;
	tmp->var=E_NULL;
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(tmp));
	return & tmp->base;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	BooleanOrder* tmp=(BooleanOrder *) ourmalloc(sizeof (BooleanOrder));
	GETBOOLEANTYPE(tmp)=ORDERCONST;
	tmp->order=order;
	tmp->first=first;
	tmp->second=second;
	//FIXME: what if client calls this function with the same arguments?
	//Instead of vector we should keep a hashtable from PAIR->BOOLEANOrder with
	//asymmetric hash function.  
	pushVectorBoolean(&order->constraints, &tmp->base);
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(tmp));
	return & tmp -> base;
}

Boolean * allocBooleanPredicate(Predicate * predicate, Element ** inputs, uint numInputs){
	BooleanPredicate* This = (BooleanPredicate*) ourmalloc(sizeof(BooleanPredicate));
	GETBOOLEANTYPE(This)= PREDICATEOP;
	This->predicate=predicate;
	allocInlineArrayInitElement(&This->inputs, inputs, numInputs);
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(This));

	for(uint i=0;i<numInputs;i++) {
		pushVectorASTNode(GETELEMENTPARENTS(inputs[i]), (ASTNode *)This);
	}
	initPredicateEncoding(&This->encoding, (Boolean *) This);

	return & This->base;
}

Boolean * allocBooleanLogicArray(CSolver *solver, LogicOp op, Boolean ** array, uint asize){
	BooleanLogic * This = ourmalloc(sizeof(BooleanLogic));
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(This));
	allocInlineArrayInitBoolean(&This->inputs, array, asize);
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
