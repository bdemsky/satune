#include "set.h"
#include <stddef.h>
#include <cassert>

Set::Set(VarType t, uint64_t* elements, int num) :
	type (t),
	isRange(false),
	low(0),
	high(0),
	members(new ModelVector<uint64_t>()) {
	members->reserve(num);
	for(int i=0;i<num;i++)
		members->push_back(elements[i]);
}

Set::Set(VarType t, uint64_t lowrange, uint64_t highrange) :
	type(t),
	isRange(true),
	low(lowrange),
	high(highrange),
	members(NULL) {
}

Set::~Set() {
	if (isRange)
		delete members;
}
