#ifndef ASTHASH_H
#define ASTHASH_H
#include "classlist.h"
#include "hashtable.h"

uint hashBoolean(Boolean * boolean);
bool compareBoolean(Boolean *b1, Boolean *b2);

uint hashElement(Element *element);
bool compareElement(Element *e1, Element *e2);

typedef HashTable<Boolean *, Boolean *, uintptr_t, 4, hashBoolean, compareBoolean> BooleanMatchMap;

typedef HashTable<Element *, Element *, uintptr_t, 4, hashElement, compareElement> ElementMatchMap;

#endif
