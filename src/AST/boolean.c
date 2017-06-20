#include "boolean.h"
#include "structs.h"
#include "csolver.h"

Boolean* allocBoolean(VarType t) {
	BooleanVar* tmp=(BooleanVar *) ourmalloc(sizeof (BooleanVar));
	GETBOOLEANTYPE(tmp)=BOOLEANVAR;
	tmp->vtype=t;
	return & tmp->base;
}

Boolean* allocBooleanOrder(Order* order, uint64_t first, uint64_t second) {
	BooleanOrder* tmp=(BooleanOrder *) ourmalloc(sizeof (BooleanOrder));
	GETBOOLEANTYPE(tmp)=ORDERCONST;
	tmp->order=order;
	tmp->first=first;
	tmp->second=second;
	return & tmp -> base;
}

Boolean * allocBooleanPredicate(Predicate * predicate, Element ** inputs, uint numInputs){
    BooleanPredicate* bp = (BooleanPredicate*) ourmalloc(sizeof(BooleanPredicate));
    GETBOOLEANTYPE(bp)= PREDICATEOP;
    bp->predicate=predicate;
    bp->inputs= allocVectorArrayElement (numInputs,inputs);
    return & bp->base;
}

Boolean * allocBooleanLogic(LogicOp op, Boolean * left, Boolean* right){
    BooleanLogic* bl = (BooleanLogic*) ourmalloc(sizeof(BooleanLogic));
    GETBOOLEANTYPE(bl) = LOGICOP;
    bl->op=op;
    bl->left=left;
    bl->right=right;
    return &bl->base;
}
Boolean * allocBooleanLogicArray(CSolver *solver, LogicOp op, Boolean ** array, uint asize){
    ASSERT(asize>=2);
    Boolean* boolean = allocBooleanLogic(op,array[0], array[1]);
    pushVectorBoolean(solver->allBooleans,boolean);
    for(uint i=2; i<asize; i++){
	boolean=allocBooleanLogic(op,boolean, array[i]);
	pushVectorBoolean(solver->allBooleans,boolean);
    }
    return boolean;
}

void deleteBoolean(Boolean * This) {
	ourfree(This);
}
