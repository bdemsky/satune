#include "boolean.h"
#include "structs.h"
#include "csolver.h"

Boolean* allocBoolean(VarType t) {
	BooleanVar* tmp=(BooleanVar *) ourmalloc(sizeof (BooleanVar));
	GETBOOLEANTYPE(tmp)=BOOLEANVAR;
	GETSTRUCTTYPE(tmp) = _BOOLEAN;
	GETPARENTSVECTOR(tmp) = allocDefVectorVoid();
	tmp->vtype=t;
	tmp->var=NULL;
	return & tmp->base;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	BooleanOrder* tmp=(BooleanOrder *) ourmalloc(sizeof (BooleanOrder));
	GETBOOLEANTYPE(tmp)=ORDERCONST;
	GETSTRUCTTYPE(tmp) = _BOOLEAN;
	GETPARENTSVECTOR(tmp) = allocDefVectorVoid();
	tmp->order=order;
	tmp->first=first;
	tmp->second=second;
	return & tmp -> base;
}

Boolean * allocBooleanPredicate(Predicate * predicate, Element ** inputs, uint numInputs){
	BooleanPredicate* tmp = (BooleanPredicate*) ourmalloc(sizeof(BooleanPredicate));
	GETBOOLEANTYPE(tmp)= PREDICATEOP;
	GETSTRUCTTYPE(tmp) = _BOOLEAN;
	GETPARENTSVECTOR(tmp) = allocDefVectorVoid();
	tmp->predicate=predicate;
	tmp->inputs= allocVectorArrayElement (numInputs,inputs);
	return & tmp->base;
}

Boolean * allocBooleanLogic(LogicOp op, Boolean * left, Boolean* right){
	BooleanLogic* tmp = (BooleanLogic*) ourmalloc(sizeof(BooleanLogic));
	GETBOOLEANTYPE(tmp) = LOGICOP;
	GETSTRUCTTYPE(tmp) = _BOOLEAN;
	GETPARENTSVECTOR(tmp) = allocDefVectorVoid();
	tmp->op=op;
	tmp->left=left;
	tmp->right=right;
	return &tmp->base;
}
Boolean * allocBooleanLogicArray(CSolver *solver, LogicOp op, Boolean ** array, uint asize){
	ASSERT(asize>=2);
	Boolean* boolean = allocBooleanLogic(op,array[0], array[1]);
	ADDNEWPARENT(array[0], boolean);
	ADDNEWPARENT(array[1], boolean);
	pushVectorBoolean(solver->allBooleans,boolean);
	for(uint i=2; i<asize; i++){
		Boolean* oldBoolean = boolean;
		boolean=allocBooleanLogic(op,oldBoolean, array[i]);
		ADDNEWPARENT(oldBoolean, boolean);
		ADDNEWPARENT(array[i], boolean);
		pushVectorBoolean(solver->allBooleans,boolean);
	}
	return boolean;
}

void deleteBoolean(Boolean * This) {
	switch(GETBOOLEANTYPE(This)){
		case PREDICATEOP:
			deleteVectorArrayElement( ((BooleanPredicate*)This)->inputs );
			break;
		default:
			break;
	}
	DELETEPARENTSVECTOR(This);
	ourfree(This);
}
