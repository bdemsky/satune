#ifndef CORESTRUCTS_H
#define CORESTRUCTS_H

#include "cppvector.h"
#include "hashset.h"

typedef Hashset<Boolean *, uintptr_t, 4> HashsetBoolean;
typedef Hashset<Order *, uintptr_t, 4> HashsetOrder;
typedef SetIterator<Boolean *, uintptr_t, 4> SetIteratorBoolean;
typedef SetIterator<Order *, uintptr_t, 4> SetIteratorOrder;

#endif
