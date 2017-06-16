#ifndef MUTABLESET_H
#define MUTABLESET_H
#include "set.h"

MutableSet * allocMutableSet(VarType t);
void addElementMSet(MutableSet * set, uint64_t element);
#endif
