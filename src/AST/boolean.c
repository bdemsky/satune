#include "boolean.h"
#include "structs.h"
#include "csolver.h"
#include "element.h"

Boolean* allocBoolean(VarType t) {
	BooleanVar* tmp=(BooleanVar *) ourmalloc(sizeof (BooleanVar));
	GETBOOLEANTYPE(tmp)=BOOLEANVAR;
	tmp->vtype=t;
	tmp->var=NULL;
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(tmp));
	return & tmp->base;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	BooleanOrder* tmp=(BooleanOrder *) ourmalloc(sizeof (BooleanOrder));
	GETBOOLEANTYPE(tmp)=ORDERCONST;
	tmp->order=order;
	tmp->first=first;
	tmp->second=second;
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(tmp));
	return & tmp -> base;
}

Boolean * allocBooleanPredicate(Predicate * predicate, Element ** inputs, uint numInputs){
	BooleanPredicate* This = (BooleanPredicate*) ourmalloc(sizeof(BooleanPredicate));
	GETBOOLEANTYPE(This)= PREDICATEOP;
	This->predicate=predicate;
	This->inputs= allocVectorArrayElement (numInputs,inputs);
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(This));

	for(uint i=0;i<numInputs;i++) {
		pushVectorASTNode(GETELEMENTPARENTS(inputs[i]), (ASTNode *)This);
	}
	return & This->base;
}

Boolean * allocBooleanLogicArray(CSolver *solver, LogicOp op, Boolean ** array, uint asize){
	BooleanLogic * This = ourmalloc(sizeof(BooleanLogic));
	allocInlineDefVectorBoolean(GETBOOLEANPARENTS(This));
	This->array = ourmalloc(sizeof(Boolean *)*asize);
	memcpy(This->array, array, sizeof(Boolean *)*asize);
	for(uint i=0;i<asize;i++) {
		pushVectorBoolean(GETBOOLEANPARENTS(array[i]), (Boolean *)This);
	}
	pushVectorBoolean(solver->allBooleans, (Boolean *) This);
	return & This->base;
}

void deleteBoolean(Boolean * This) {
	switch(GETBOOLEANTYPE(This)){
		case PREDICATEOP:
			deleteVectorArrayElement( ((BooleanPredicate*)This)->inputs );
			break;
		default:
			break;
	}
	deleteVectorArrayBoolean(GETBOOLEANPARENTS(This));
	ourfree(This);
}
