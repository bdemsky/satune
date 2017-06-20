#include "naiveelementencoder.h"
#include "elementencoding.h"
#include "element.h"
#include "set.h"
#include "common.h"
#include "structs.h"

void baseBinaryIndexElementAssign(ElementEncoding *This) {
	Element * element=This->element;
	Set * set=element->set;
	ASSERT(set->isRange==false);
	uint size=getSizeVectorInt(set->members);
	This->encodingArray=ourmalloc(sizeof(uint64_t)*size);
	for(uint i=0;i<size;i++) {
		This->encodingArray[i]=getVectorInt(set->members, i);
	}
	This->type=BINARYINDEX;
}
