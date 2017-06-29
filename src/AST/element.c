#include "element.h"
#include "structs.h"
#include "set.h"
#include "constraint.h"
#include "function.h"
#include "table.h"

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

Set* getElementSet(Element* This){
	switch(GETELEMENTTYPE(This)){
		case ELEMSET:
			return ((ElementSet*)This)->set;
		case ELEMFUNCRETURN:
			;//Nope is needed for label assignment. i.e. next instruction isn't 
			Function* func = ((ElementFunction*)This)->function;
			switch(GETFUNCTIONTYPE(func)){
				case TABLEFUNC:
					return ((FunctionTable*)func)->table->range;
				case OPERATORFUNC:
					return ((FunctionOperator*)func)->range;
				default:
					ASSERT(0);
			}
		default:
			ASSERT(0);
	}
	ASSERT(0);
	return NULL;
}

uint getElemEncodingInUseVarsSize(ElementEncoding* This){
	uint size=0;
	for(uint i=0; i<This->encArraySize; i++){
		if(isinUseElement(This, i)){
			size++;
		}
	}
	return size;
}


Constraint * getElementValueBinaryIndexConstraint(Element* This, uint64_t value) {
	ASTNodeType type = GETELEMENTTYPE(This);
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding* elemEnc = getElementEncoding(This);
	for(uint i=0; i<elemEnc->encArraySize; i++){
		if( isinUseElement(elemEnc, i) && elemEnc->encodingArray[i]==value){
			return generateBinaryConstraint(elemEnc->numVars,
				elemEnc->variables, i);
		}
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
		;
	}
	deleteVectorArrayASTNode(GETELEMENTPARENTS(This));

	ourfree(This);
}
