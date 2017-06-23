
#include "naivefunctionencoder.h"
#include "functionencoding.h"
#include "common.h"
#include "element.h"
#include "boolean.h"
#include "function.h"
#include "table.h"
#include "tableentry.h"

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
	VectorElement* elements= This->Elements;
	Table* table = ((FunctionTable*) This->function)->table;
	uint size = getSizeVectorTableEntry(table->entries);
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(table->entries, i);
		//FIXME: generate Constraints
	}
	
}

void naiveEncodeEnumOperatingFunc(Encodings* encodings, ElementFunction* This){
	
}