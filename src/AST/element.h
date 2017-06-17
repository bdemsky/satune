#ifndef ELEMENT_H
#define ELEMENT_H
#include "classlist.h"
#include "mymemory.h"

struct Element {
	Set * set;
};

Element * allocElement(Set *s);
void deleteElement(Element *This);
#endif
