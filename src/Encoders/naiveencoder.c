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
#include "order.h"
#include <strings.h>


void naiveEncodingDecision(CSolver* csolver){
	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		//Whether it's a ElementFunction or ElementSet we should do the followings:
		setElementEncodingType(getElementEncoding(element), BINARYINDEX);
		baseBinaryIndexElementAssign(getElementEncoding(element));
		switch(GETELEMENTTYPE(element)){
			case ELEMSET:
				//FIXME: Move next line to satEncoderInitializer!
//				generateElementEncodingVariables(encoder,getElementEncoding(element));
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
		Boolean* boolean = getVectorBoolean(csolver->allBooleans, i);
		switch(GETBOOLEANTYPE(boolean)){
			case PREDICATEOP:
				setFunctionEncodingType(getPredicateFunctionEncoding((BooleanPredicate*)boolean),
					ENUMERATEIMPLICATIONS);
				break;
			case ORDERCONST:
				setOrderEncodingType( ((BooleanOrder*)boolean)->order, PAIRWISE );
				break;
			default:
				continue;
		} 
	}
}

void baseBinaryIndexElementAssign(ElementEncoding *This) {
	Element * element=This->element;
	Set * set= getElementSet(element);
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


