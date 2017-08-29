#ifndef ASTHASH_H
#define ASTHASH_H
#include "classlist.h"

uint hashBoolean(Boolean * boolean);
bool compareBoolean(Boolean *b1, Boolean *b2);

uint hashElement(Element *element);
bool compareElement(Element *e1, Element *e2);

#endif
