#include "element.h"
#include "structs.h"

Element *allocElementSet(Set * s) {
	ElementSet * tmp=(ElementSet *)ourmalloc(sizeof(ElementSet));
	GETELEMENTTYPE(tmp)= ELEMSET;
	tmp->set=s;
	return &tmp->base;
}

Element* allocElementFunction(Function * function, Element ** array, uint numArrays, Boolean * overflowstatus){
    ElementFunction* ef = (ElementFunction*) ourmalloc(sizeof(ElementFunction));
    GETELEMENTTYPE(ef)= ELEMFUNCRETURN;
    ef->function=function;
    ef->overflowstatus = overflowstatus;
    ef->Elements = allocVectorArrayElement(numArrays, array);
    return &ef->base;
}

void deleteElement(Element *This) {
	ourfree(This);
}
