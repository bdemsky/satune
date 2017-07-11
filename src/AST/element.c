#include "element.h"
#include "structs.h"
#include "set.h"
#include "constraint.h"
#include "function.h"
#include "table.h"

Element *allocElementSet(Set * s) {
	ElementSet * This=(ElementSet *)ourmalloc(sizeof(ElementSet));
	GETELEMENTTYPE(This)= ELEMSET;
	This->set=s;
	allocInlineDefVectorASTNode(GETELEMENTPARENTS(This));
	initElementEncoding(&This->encoding, (Element *) This);
	return &This->base;
}

Element* allocElementFunction(Function * function, Element ** array, uint numArrays, Boolean * overflowstatus){
	ElementFunction* This = (ElementFunction*) ourmalloc(sizeof(ElementFunction));
	GETELEMENTTYPE(This)= ELEMFUNCRETURN;
	This->function=function;
	This->overflowstatus = overflowstatus;
	allocInlineArrayInitElement(&This->inputs, array, numArrays);
	allocInlineDefVectorASTNode(GETELEMENTPARENTS(This));
	for(uint i=0;i<numArrays;i++)
		pushVectorASTNode(GETELEMENTPARENTS(array[i]), (ASTNode *) This);
	initElementEncoding(&This->domainencoding, (Element *) This);
	initFunctionEncoding(&This->functionencoding, (Element *) This);
	return &This->base;
}

Set* getElementSet(Element* This){
	switch(GETELEMENTTYPE(This)){
	case ELEMSET:
		return ((ElementSet*)This)->set;
	case ELEMFUNCRETURN: {
		Function* func = ((ElementFunction*)This)->function;
		switch(GETFUNCTIONTYPE(func)){
		case TABLEFUNC:
			return ((FunctionTable*)func)->table->range;
		case OPERATORFUNC:
			return ((FunctionOperator*)func)->range;
		default:
			ASSERT(0);
		}
	}
	default:
		ASSERT(0);
	}
	ASSERT(0);
	return NULL;
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
		ASSERT(0);
	}
	deleteVectorArrayASTNode(GETELEMENTPARENTS(This));
	ourfree(This);
}
