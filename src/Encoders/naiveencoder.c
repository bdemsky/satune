#include "naiveencoder.h"
#include "elementencoding.h"
#include "element.h"
#include "functionencoding.h"
#include "function.h"
#include "set.h"
#include "common.h"
#include "structs.h"
#include <strings.h>

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


void naiveEncodeFunctionPredicate(Encodings* encodings, FunctionEncoding *This){
	if(This->isFunction) {
		ASSERT(GETELEMENTTYPE(This->op.function)==ELEMFUNCRETURN);
		if(This->type==CIRCUIT){
			naiveEncodeCircuitFunction(encodings, This);
		} else if( This->type == ENUMERATEIMPLICATIONS){
			naiveEncodeEnumeratedFunction(encodings, This);
		} else
			ASSERT(0);
			
	}else {
		ASSERT(GETBOOLEANTYPE(This->op.predicate) == PREDICATEOP);
		BooleanPredicate* predicate = (BooleanPredicate*)This->op.predicate;
		//FIXME
		
	}
}


void naiveEncodeCircuitFunction(Encodings* encodings, FunctionEncoding* This){
	
}

void naiveEncodeEnumeratedFunction(Encodings* encodings, FunctionEncoding* This){
	ElementFunction* ef =(ElementFunction*)This->op.function;
	Function * function = ef->function;
	if(GETFUNCTIONTYPE(function)==TABLEFUNC){
		naiveEncodeEnumTableFunc(encodings, ef);
	}else if (GETFUNCTIONTYPE(function)== OPERATORFUNC){
		naiveEncodeEnumOperatingFunc(encodings, ef);
	}else 
		ASSERT(0);
}

void naiveEncodeEnumTableFunc(Encodings* encodings, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function)==TABLEFUNC);
	ArrayElement* elements= &This->inputs;
	Table* table = ((FunctionTable*) (This->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		//FIXME: generate Constraints
	}
	
}

void naiveEncodeEnumOperatingFunc(Encodings* encodings, ElementFunction* This){
	
}