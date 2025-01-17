#include "set.h"
#include <stddef.h>
#include "csolver.h"
#include "serializer.h"
#include "qsort.h"

int intcompare(const void *p1, const void *p2) {
	uint64_t a = *(uint64_t const *) p1;
	uint64_t b = *(uint64_t const *) p2;
	if (a < b)
		return -1;
	else if (a == b)
		return 0;
	else
		return 1;
}

Set::Set(VarType t) : type(t), isRange(false), low(0), high(0) {
	members = new Vector<uint64_t>();
}


Set::Set(VarType t, uint64_t *elements, uint num) : type(t), isRange(false), low(0), high(0) {
	members = new Vector<uint64_t>(num, elements);
	bsdqsort(members->expose(), members->getSize(), sizeof(uint64_t), intcompare);
}


Set::Set(VarType t, uint64_t lowrange, uint64_t highrange) : type(t), isRange(true), low(lowrange), high(highrange), members(NULL) {
}

bool Set::exists(uint64_t element) {
	if (isRange) {
		return element >= low && element <= high;
	} else {
		//Use Binary Search
		uint low = 0;
		uint high = members->getSize() - 1;
		while (true) {
			uint middle = (low + high) / 2;
			uint64_t val = members->get(middle);
			if (element < val) {
				high = middle - 1;
				if (middle <= low)
					return false;
			} else if (element > val) {
				low = middle + 1;
				if (middle >= high)
					return false;
			} else {
				return true;
			}
		}
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

uint Set::getUnionSize(Set *s) {
	uint sSize = s->getSize();
	uint thisSize = getSize();
	uint sIndex = 0;
	uint thisIndex = 0;
	uint sum = 0;
	uint64_t sValue = s->getElement(sIndex);
	uint64_t thisValue = getElement(thisIndex);
	while (thisIndex < thisSize && sIndex < sSize) {
		if (sValue < thisValue) {
			sIndex++;
			if (sIndex < sSize)
				sValue = s->getElement(sIndex);
			sum++;
		} else if (thisValue < sValue) {
			thisIndex++;
			if (thisIndex < thisSize)
				thisValue = getElement(thisIndex);
			sum++;
		} else {
			thisIndex++;
			sIndex++;
			if (sIndex < sSize)
				sValue = s->getElement(sIndex);
			if (thisIndex < thisSize)
				thisValue = getElement(thisIndex);
			sum++;
		}
	}
	sum += (thisSize - thisIndex) + (sSize - sIndex);

	return sum;
}

void Set::serialize(Serializer *serializer) {
	if (serializer->isSerialized(this))
		return;
	serializer->addObject(this);
	ASTNodeType asttype = SETTYPE;
	serializer->mywrite(&asttype, sizeof(ASTNodeType));
	Set *This = this;
	serializer->mywrite(&This, sizeof(Set *));
	serializer->mywrite(&type, sizeof(VarType));
	serializer->mywrite(&isRange, sizeof(bool));
	bool isMutable = isMutableSet();
	serializer->mywrite(&isMutable, sizeof(bool));
	if (isRange) {
		serializer->mywrite(&low, sizeof(uint64_t));
		serializer->mywrite(&high, sizeof(uint64_t));
	} else {
		uint size = members->getSize();
		serializer->mywrite(&size, sizeof(uint));
		for (uint i = 0; i < size; i++) {
			uint64_t mem = members->get(i);
			serializer->mywrite(&mem, sizeof(uint64_t));
		}
	}
}

void Set::print() {
	model_print("{Set(%lu)<%p>:", type, this);
	if (isRange) {
		model_print("Range: low=%lu, high=%lu}", low, high);
	} else {
		uint size = members->getSize();
		model_print("Members: ");
		for (uint i = 0; i < size; i++) {
			uint64_t mem = members->get(i);
			model_print("%lu, ", mem);
		}
		model_print("}");
	}
}
