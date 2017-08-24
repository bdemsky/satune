#include "element.h"
#include "structs.h"
#include "set.h"
#include "constraint.h"
#include "function.h"
#include "table.h"

Element *allocElementSet(Set *s) {
	ElementSet *This = (ElementSet *)ourmalloc(sizeof(ElementSet));
	GETELEMENTTYPE(This) = ELEMSET;
	This->set = s;
	initDefVectorASTNode(GETELEMENTPARENTS(This));
	initElementEncoding(&This->encoding, (Element *) This);
	return &This->base;
}

Element *allocElementFunction(Function *function, Element **array, uint numArrays, Boolean *overflowstatus) {
	ElementFunction *This = (ElementFunction *) ourmalloc(sizeof(ElementFunction));
	GETELEMENTTYPE(This) = ELEMFUNCRETURN;
	This->function = function;
	ASSERT(GETBOOLEANTYPE(overflowstatus) == BOOLEANVAR);
	This->overflowstatus = overflowstatus;
	initArrayInitElement(&This->inputs, array, numArrays);
	initDefVectorASTNode(GETELEMENTPARENTS(This));
	for (uint i = 0; i < numArrays; i++)
		pushVectorASTNode(GETELEMENTPARENTS(array[i]), (ASTNode *) This);
	initElementEncoding(&This->rangeencoding, (Element *) This);
	initFunctionEncoding(&This->functionencoding, (Element *) This);
	return &This->base;
}

Element *allocElementConst(uint64_t value, VarType type) {
	ElementConst *This = (ElementConst *)ourmalloc(sizeof(ElementConst));
	GETELEMENTTYPE(This) = ELEMCONST;
	This->value = value;
	This->set = allocSet(type, (uint64_t[]) {value}, 1);
	initDefVectorASTNode(GETELEMENTPARENTS(This));
	initElementEncoding(&This->encoding, (Element *) This);
	return &This->base;
}

Set *getElementSet(Element *This) {
	switch (GETELEMENTTYPE(This)) {
	case ELEMSET:
		return ((ElementSet *)This)->set;
	case ELEMCONST:
		return ((ElementConst *)This)->set;
	case ELEMFUNCRETURN: {
		Function *func = ((ElementFunction *)This)->function;
		switch (GETFUNCTIONTYPE(func)) {
		case TABLEFUNC:
			return ((FunctionTable *)func)->table->range;
		case OPERATORFUNC:
			return ((FunctionOperator *)func)->range;
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
	switch (GETELEMENTTYPE(This)) {
	case ELEMFUNCRETURN: {
		ElementFunction *ef = (ElementFunction *) This;
		deleteInlineArrayElement(&ef->inputs);
		deleteElementEncoding(&ef->rangeencoding);
		deleteFunctionEncoding(&ef->functionencoding);
		break;
	}
	case ELEMSET: {
		ElementSet *es = (ElementSet *) This;
		deleteElementEncoding(&es->encoding);
		break;
	}
	case ELEMCONST: {
		ElementConst *ec = (ElementConst *) This;
		deleteSet(ec->set);//Client did not create, so we free it
		deleteElementEncoding(&ec->encoding);
		break;
	}
	default:
		ASSERT(0);
	}
	deleteVectorArrayASTNode(GETELEMENTPARENTS(This));
	ourfree(This);
}
