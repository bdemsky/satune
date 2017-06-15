#include "element.h"

Element *allocElement(Set * s) {
	Element * tmp=(Element *)ourmalloc(sizeof(Element));
	tmp->set=s;
	return tmp;
}
