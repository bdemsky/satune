#ifndef MUTABLESET_H
#define MUTABLESET_H
#include "set.h"

class MutableSet : Set {
public:
	void addElement(uint64_t element) { members->push_back(element); }
};
#endif
