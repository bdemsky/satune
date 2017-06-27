#include "element.h"
#include "structs.h"
#include "set.h"
#include "constraint.h"

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

uint getElementSize(Element* This){
	switch(GETELEMENTTYPE(This)){
		case ELEMSET:
			return getSetSize( ((ElementSet*)This)->set );
			break;
		case ELEMFUNCRETURN:
			ASSERT(0);
		default:
			ASSERT(0);
	}
	return -1;
}


Constraint * getElementValueConstraint(Element* This, uint64_t value) {
	switch(GETELEMENTTYPE(This)){
		case ELEMSET:
			; //Statement is needed for a label and This is a NOPE
			ElementSet* elemSet= ((ElementSet*)This);
			uint size = getSetSize(elemSet->set);
			for(uint i=0; i<size; i++){
				if( getElementEncoding(elemSet)->encodingArray[i]==value){
					return generateBinaryConstraint(getElementEncoding(elemSet)->numVars,
						getElementEncoding(elemSet)->variables, i);
				}
			}
			break;
		case ELEMFUNCRETURN:
			break;
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
		;
	}
	deleteVectorArrayASTNode(GETELEMENTPARENTS(This));

	ourfree(This);
}
