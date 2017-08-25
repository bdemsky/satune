#include "mutableset.h"

MutableSet::MutableSet(VarType t) : Set(t) {
}

void MutableSet::addElementMSet(uint64_t element) {
	members->push(element);
}
