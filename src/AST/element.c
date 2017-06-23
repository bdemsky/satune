#include "element.h"
#include "structs.h"

Element *allocElementSet(Set * s) {
	ElementSet * tmp=(ElementSet *)ourmalloc(sizeof(ElementSet));
	GETELEMENTTYPE(tmp)= ELEMSET;
	tmp->set=s;
	allocInlineDefVectorASTNode(GETELEMENTPARENTS(tmp));
	initElementEncoding(&tmp->encoding, (Element *) tmp);
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
	initElementEncoding(&tmp->domainencoding, (Element *) tmp);
	initFunctionEncoding(&tmp->functionencoding, (Element *) tmp);
	return &tmp->base;
}

void deleteElement(Element *This) {
	switch(GETELEMENTTYPE(This)) {
	case ELEMFUNCRETURN: {
		ElementFunction *ef = (ElementFunction *) This;
		deleteInlineArrayElement(&ef->inputs);
		deleteElementEncoding(&ef->domainencoding);
		deleteFunctionEncoding(&ef->functionencoding);
		break;
	}
	case ELEMSET: {
		ElementSet *es = (ElementSet *) This;
		deleteElementEncoding(&es->encoding);
		break;
	}
	default:
		;
	}
	deleteVectorArrayASTNode(GETELEMENTPARENTS(This));

	ourfree(This);
}
