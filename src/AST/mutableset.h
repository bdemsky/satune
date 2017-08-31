#ifndef MUTABLESET_H
#define MUTABLESET_H
#include "set.h"

class MutableSet : public Set {
public:
	MutableSet(VarType t);
	void addElementMSet(uint64_t element);
	Set *clone(CSolver *solver, CloneMap *map);
	CMEMALLOC;
};
#endif
