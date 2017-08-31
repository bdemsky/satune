#ifndef CORESTRUCTS_H
#define CORESTRUCTS_H

#include "cppvector.h"
#include "hashset.h"

typedef Hashset<Boolean *, uintptr_t, 4> HashsetBoolean;
typedef SetIterator<Boolean *, uintptr_t, 4> SetIteratorBoolean;

#endif
