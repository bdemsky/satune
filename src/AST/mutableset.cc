#include "mutableset.h"
#include "csolver.h"

MutableSet::MutableSet(VarType t) : Set(t) {
}

void MutableSet::addElementMSet(uint64_t element) {
	members->push(element);
}

Set *MutableSet::clone(CSolver *solver, CloneMap *map) {
	Set *s = (Set *) map->get(this);
	if (s != NULL)
		return s;
	s = solver->createMutableSet(type);
	for (uint i = 0; i < members->getSize(); i++) {
		solver->addItem((MutableSet *) s, members->get(i));
	}
	map->put(this, s);
	return s;
}
