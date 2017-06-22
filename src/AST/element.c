#include "element.h"
#include "structs.h"

Element *allocElementSet(Set * s) {
	ElementSet * tmp=(ElementSet *)ourmalloc(sizeof(ElementSet));
	GETELEMENTTYPE(tmp)= ELEMSET;
	tmp->set=s;
	tmp->encoding=NULL;
	allocInlineDefVectorASTNode(GETELEMENTPARENTS(tmp));
	return &tmp->base;
}

Element* allocElementFunction(Function * function, Element ** array, uint numArrays, Boolean * overflowstatus){
	ElementFunction* tmp = (ElementFunction*) ourmalloc(sizeof(ElementFunction));
	GETELEMENTTYPE(tmp)= ELEMFUNCRETURN;
	tmp->function=function;
	tmp->overflowstatus = overflowstatus;
	allocInlineArrayInitElement(&tmp->inputs, array, numArrays);
	allocInlineDefVectorASTNode(GETELEMENTPARENTS(tmp));
	for(uint i=0;i<numArrays;i++)
		pushVectorASTNode(GETELEMENTPARENTS(array[i]), (ASTNode *) tmp);
	return &tmp->base;
}

void deleteElement(Element *This) {
	switch(GETELEMENTTYPE(This)) {
	case ELEMFUNCRETURN:
		deleteInlineArrayElement(&((ElementFunction *)This)->inputs);
		break;
	default:
		;
	}
	deleteVectorArrayASTNode(GETELEMENTPARENTS(This));
	ourfree(This);
}
