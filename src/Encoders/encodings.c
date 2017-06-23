#include "encodings.h"
#include "elementencoding.h"
#include "functionencoding.h"
#include "element.h"
#include "common.h"
#include "boolean.h"
#include "naiveelementencoder.h"

Encodings* allocEncodings(){
	Encodings* This = (Encodings*) ourmalloc(sizeof(Encodings));
	allocInlineDefVectorElementEncoding(GETVECTORELEMENTENCODING(This));
	allocInlineDefVectorFunctionEncoding(GETVECTORFUNCTIONENCODING(This));
	This->elemToEncode= allocHashTableElemToEncod(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	This->voidToFuncEncode= allocHashTableVoidToFuncEncod(HT_INITIAL_CAPACITY, HT_DEFAULT_FACTOR);
	return This;
}

void deleteEncodings(Encodings* This){
	deleteVectorArrayFunctionEncoding(GETVECTORFUNCTIONENCODING(This));
	deleteVectorArrayElementEncoding(GETVECTORELEMENTENCODING(This));
	deleteHashTableElemToEncod(This->elemToEncode);
	deleteHashTableVoidToFuncEncod(This->voidToFuncEncode);
	ourfree(This);
}

void assignEncoding(CSolver* csolver, Encodings* This){
	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		if(GETELEMENTTYPE(element)==ELEMSET){
			ElementEncoding* eencoding = allocElementEncoding( BINARYINDEX, element);
			baseBinaryIndexElementAssign(eencoding);
			pushVectorElementEncoding(GETVECTORELEMENTENCODING(This) , eencoding);
			putElemToEncod(This->elemToEncode, element, eencoding); 
		}else if (GETELEMENTTYPE(element)==ELEMFUNCRETURN){
			FunctionEncoding* fencoding = allocFunctionEncoding( ENUMERATEIMPLICATIONS, element);
			pushVectorFunctionEncoding(GETVECTORFUNCTIONENCODING(This) , fencoding);
			putVoidToFuncEncod(This->voidToFuncEncode,element, fencoding);
		}else
			ASSERT(0);
	}
	
	size = getSizeVectorBoolean(csolver->allBooleans);
	for(uint i=0; i<size; i++){
		Boolean* predicate = getVectorBoolean(csolver->allBooleans, i);
		if(GETBOOLEANTYPE(predicate)==PREDICATEOP){
			FunctionEncoding* fencoding = allocPredicateEncoding(ENUMERATEIMPLICATIONS, predicate);
			pushVectorFunctionEncoding(GETVECTORFUNCTIONENCODING(This), fencoding);
			putVoidToFuncEncod(This->voidToFuncEncode, predicate,fencoding);
		}else 
			ASSERT(0);
	}
}

void encodeFunctionsElements(Encodings* This){
	//call encoding for each element/predicate
}