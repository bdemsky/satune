#include "naiveelementencoder.h"
#include "elementencoding.h"
#include "element.h"
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



