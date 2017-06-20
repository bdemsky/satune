#include "element.h"
#include "structs.h"
//FIXME: ELEMENTSET?
Element *allocElement(Set * s) {
	Element * tmp=(Element *)ourmalloc(sizeof(Element));
	GETELEMENTTYPE(tmp)= ELEMSET;
	tmp->set=s;
	tmp->encoding=NULL;
	return tmp;
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
