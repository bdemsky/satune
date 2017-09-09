#include "set.h"
#include <stddef.h>
#include "csolver.h"
#include "serializer.h"

Set::Set(VarType t) : type(t), isRange(false), low(0), high(0) {
	members = new Vector<uint64_t>();
}

Set::Set(VarType t, uint64_t *elements, uint num) : type(t), isRange(false), low(0), high(0) {
	members = new Vector<uint64_t>(num, elements);
}

Set::Set(VarType t, uint64_t lowrange, uint64_t highrange) : type(t), isRange(true), low(lowrange), high(highrange), members(NULL) {
}

bool Set::exists(uint64_t element) {
	if (isRange) {
		return element >= low && element <= high;
	} else {
		uint size = members->getSize();
		for (uint i = 0; i < size; i++) {
			if (element == members->get(i))
				return true;
		}
		return false;
	}
}

uint64_t Set::getElement(uint index) {
	if (isRange)
		return low + index;
	else
		return members->get(index);
}

uint Set::getSize() {
	if (isRange) {
		return high - low + 1;
	} else {
		return members->getSize();
	}
}

uint64_t Set::getMemberAt(uint index) {
	if (isRange) {
		return low + index;
	} else {
		return members->get(index);
	}
}

Set::~Set() {
	if (!isRange)
		delete members;
}

Set *Set::clone(CSolver *solver, CloneMap *map) {
	Set *s = (Set *) map->get(this);
	if (s != NULL)
		return s;
	if (isRange) {
		s = solver->createRangeSet(type, low, high);
	} else {
		s = solver->createSet(type, members->expose(), members->getSize());
	}
	map->put(this, s);
	return s;
}


void Set::serialize(Serializer* serializer){
	if(serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	ASTNodeType asttype = SETTYPE;
	serializer->mywrite(&asttype, sizeof(ASTNodeType));
	Set* This = this;
	serializer->mywrite(&This, sizeof(Set*));
	serializer->mywrite(&type, sizeof(VarType));
	serializer->mywrite(&isRange, sizeof(bool));
	serializer->mywrite(&low, sizeof(uint64_t));
	serializer->mywrite(&high, sizeof(uint64_t));
	uint size = members->getSize();
	serializer->mywrite(&size, sizeof(uint));
	for(uint i=0; i<size; i++){
		uint64_t mem = members->get(i);
		serializer->mywrite(&mem, sizeof(uint64_t));
	}
}