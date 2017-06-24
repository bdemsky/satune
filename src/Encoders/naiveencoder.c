#include "naiveencoder.h"
#include "elementencoding.h"
#include "element.h"
#include "functionencoding.h"
#include "function.h"
#include "set.h"
#include "common.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "table.h"
#include <strings.h>

void naiveEncodingDecision(CSolver* csolver){
	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		switch(GETELEMENTTYPE(element)){
			case ELEMSET:
				setElementEncodingType(&((ElementSet*)element)->encoding, BINARYINDEX);
				baseBinaryIndexElementAssign(&((ElementSet*)element)->encoding);
				break;
			case ELEMFUNCRETURN: 
				setFunctionEncodingType(&((ElementFunction*)element)->functionencoding, ENUMERATEIMPLICATIONS);
				break;
			default:
				ASSERT(0);
		}
	}
	
	size = getSizeVectorBoolean(csolver->allBooleans);
	for(uint i=0; i<size; i++){
		Boolean* predicate = getVectorBoolean(csolver->allBooleans, i);
		switch(GETBOOLEANTYPE(predicate)){
			case PREDICATEOP:
				setFunctionEncodingType(&((BooleanPredicate*)predicate)->encoding, ENUMERATEIMPLICATIONS);
				break;
			default:
				continue;
		} 
	}
}

void baseBinaryIndexElementAssign(ElementEncoding *This) {
	Element * element=This->element;
	ASSERT(element->type == ELEMSET);
	Set * set= ((ElementSet*)element)->set;
	ASSERT(set->isRange==false);
	uint size=getSizeVectorInt(set->members);
	uint encSize=NEXTPOW2(size);
	allocEncodingArrayElement(This, encSize);
	allocInUseArrayElement(This, encSize);

	for(uint i=0;i<size;i++) {
		This->encodingArray[i]=getVectorInt(set->members, i);
		setInUseElement(This, i);
	}
}


void encode(CSolver* csolver){
	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		switch(GETELEMENTTYPE(element)){
			case ELEMFUNCRETURN: 
				naiveEncodeFunctionPredicate(&((ElementFunction*)element)->functionencoding);
				break;
			default:
				continue;
		}
	}
	
	size = getSizeVectorBoolean(csolver->allBooleans);
	for(uint i=0; i<size; i++){
		Boolean* predicate = getVectorBoolean(csolver->allBooleans, i);
		switch(GETBOOLEANTYPE(predicate)){
			case PREDICATEOP:
				naiveEncodeFunctionPredicate(&((BooleanPredicate*)predicate)->encoding);
				break;
			default:
				continue;
		} 
	}
}

void naiveEncodeFunctionPredicate(FunctionEncoding *This){
	if(This->isFunction) {
		ASSERT(GETELEMENTTYPE(This->op.function)==ELEMFUNCRETURN);
		switch(This->type){
			case ENUMERATEIMPLICATIONS:
				naiveEncodeEnumeratedFunction(This);
				break;
			case CIRCUIT:
				naiveEncodeCircuitFunction(This);
				break;
			default:
				ASSERT(0);
		}
	}else {
		ASSERT(GETBOOLEANTYPE(This->op.predicate) == PREDICATEOP);
		BooleanPredicate* predicate = (BooleanPredicate*)This->op.predicate;
		//FIXME
		
	}
}


void naiveEncodeCircuitFunction(FunctionEncoding* This){
	
}

void naiveEncodeEnumeratedFunction(FunctionEncoding* This){
	ElementFunction* ef =(ElementFunction*)This->op.function;
	Function * function = ef->function;
	switch(GETFUNCTIONTYPE(function)){
		case TABLEFUNC:
			naiveEncodeEnumTableFunc(ef);
			break;
		case OPERATORFUNC:
			naiveEncodeEnumOperatingFunc(ef);
			break;
		default:
			ASSERT(0);
	} 
}

void naiveEncodeEnumTableFunc(ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function)==TABLEFUNC);
	ArrayElement* elements= &This->inputs;
	Table* table = ((FunctionTable*) (This->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		//FIXME: generate Constraints
	}
	
}

void naiveEncodeEnumOperatingFunc(ElementFunction* This){
	
}