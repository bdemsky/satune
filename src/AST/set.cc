#include "set.h"
#include <stddef.h>

Set::Set(VarType t, uint64_t *elements, uint num) : type(t), isRange(false), low(0), high(0) {
	members = allocVectorArrayInt(num, elements);
}

Set::Set(VarType t, uint64_t lowrange, uint64_t highrange) : type(t), isRange(true), low(lowrange), high(highrange), members(NULL) {
}

bool Set::exists(uint64_t element) {
	if (isRange) {
		return element >= low && element <= high;
	} else {
		uint size = getSizeVectorInt(members);
		for (uint i = 0; i < size; i++) {
			if (element == getVectorInt(members, i))
				return true;
		}
		return false;
	}
}

uint64_t Set::getElement(uint index) {
	if (isRange)
		return low + index;
	else
		return getVectorInt(members, index);
}

uint Set::getSize() {
	if (isRange) {
		return high - low + 1;
	} else {
		return getSizeVectorInt(members);
	}
}

Set::~Set() {
	if (!isRange)
		deleteVectorInt(members);
}
