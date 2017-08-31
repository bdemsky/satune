#ifndef ASTHASH_H
#define ASTHASH_H
#include "classes.h"
#include "hashtable.h"

uint hashBoolean(Boolean * boolean);
bool compareBoolean(Boolean *b1, Boolean *b2);

uint hashElement(Element *element);
bool compareElement(Element *e1, Element *e2);

typedef Hashtable<Boolean *, Boolean *, uintptr_t, 4, hashBoolean, compareBoolean> BooleanMatchMap;

typedef Hashtable<Element *, Element *, uintptr_t, 4, hashElement, compareElement> ElementMatchMap;

#endif
