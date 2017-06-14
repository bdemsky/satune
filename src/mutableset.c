#include "mutableset.h"

void addElementMSet(MutableSet * set, uint64_t element) { pushVectorInt(set->members, element); }
