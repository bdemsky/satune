#include "element.h"
#include "structs.h"

Element *allocElementSet(Set * s) {
	ElementSet * tmp=(ElementSet *)ourmalloc(sizeof(ElementSet));
	GETELEMENTTYPE(tmp)= ELEMSET;
	GETPARENTSVECTOR(tmp) = allocDefVectorVoid();
	GETSTRUCTTYPE(tmp) = _ELEMENT;
	tmp->set=s;
	tmp->encoding=NULL;
	return &tmp->base;
}

Element* allocElementFunction(Function * function, Element ** array, uint numArrays, Boolean * overflowstatus){
	ElementFunction* tmp = (ElementFunction*) ourmalloc(sizeof(ElementFunction));
	GETELEMENTTYPE(tmp)= ELEMFUNCRETURN;
	GETPARENTSVECTOR(tmp) = allocDefVectorVoid();
	GETSTRUCTTYPE(tmp) = _ELEMENT;
	tmp->function=function;
	tmp->overflowstatus = overflowstatus;
	tmp->Elements = allocVectorArrayElement(numArrays, array);
	return &tmp->base;
}

void deleteElement(Element *This) {
	DELETEPARENTSVECTOR(This);
	ourfree(This);
}
