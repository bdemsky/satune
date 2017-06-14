#ifndef ELEMENT_H
#define ELEMENT_H
#include "classlist.h"
#include "mymemory.h"

class Element {
public:
	Element(Set *s);
	MEMALLOC;
private:
	Set *set;
};
#endif
