#include "element.h"

Element *allocElement(Set * s) {
	Element * tmp=(Element *)ourmalloc(sizeof(Element));
	tmp->set=s;
	tmp->encoding=NULL;
	return tmp;
}

void deleteElement(Element *This) {
	ourfree(This);
}
