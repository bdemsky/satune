#include "element.h"

Element *allocElement(Set * s) {
	Element * tmp=(Element *)ouralloc(sizeof(Element));
	tmp->set=s;
	return tmp;
}
