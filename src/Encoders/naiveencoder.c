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
#include "tableentry.h"
#include <strings.h>


void naiveEncodingDecision(CSolver* csolver, SATEncoder* encoder){
	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		switch(GETELEMENTTYPE(element)){
			case ELEMSET:
				setElementEncodingType(getElementEncoding(element), BINARYINDEX);
				//FIXME:Should be moved to SATEncoder
				baseBinaryIndexElementAssign(getElementEncoding(element));
				generateElementEncodingVariables(encoder,getElementEncoding(element));
				//
				break;
			case ELEMFUNCRETURN: 
				setFunctionEncodingType(getElementFunctionEncoding((ElementFunction*)element),
					ENUMERATEIMPLICATIONS);
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
				setFunctionEncodingType(getPredicateFunctionEncoding((BooleanPredicate*)predicate),
					ENUMERATEIMPLICATIONS);
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
	This->numVars = NUMBITS(size-1);
	This->variables = ourmalloc(sizeof(Constraint*)* This->numVars);
	
	
}


