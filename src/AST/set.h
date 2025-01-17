/*
 * File:   set.h
 * Author: hamed
 *
 * Created on June 13, 2017, 3:01 PM
 */

#ifndef SET_H
#define SET_H

#include "classlist.h"
#include "structs.h"
#include "mymemory.h"

class Set {
public:
	Set(VarType t);
	Set(VarType t, uint64_t *elements, uint num);
	Set(VarType t, uint64_t lowrange, uint64_t highrange);
	virtual ~Set();
	bool exists(uint64_t element);
	uint getSize();
	VarType getType() {return type;}
	uint64_t getNewUniqueItem() {return low++;}
	uint64_t getElement(uint index);
	uint getUnionSize(Set *s);
	virtual bool isMutableSet() {return false;}
	virtual Set *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer *serializer);
	virtual void print();
	CMEMALLOC;
protected:
	VarType type;
	bool isRange;
	uint64_t low;	//also used to count unique items
	uint64_t high;
	Vector<uint64_t> *members;
	friend class ElementOpt;
};

int intcompare(const void *p1, const void *p2);
#endif/* SET_H */

