/*
 * File:   set.h
 * Author: hamed
 *
 * Created on June 13, 2017, 3:01 PM
 */

#ifndef SET_H
#define SET_H

#include "classlist.h"
#include "stl-model.h"
#include "mymemory.h"

class Set {
public:
	Set(VarType t, uint64_t * elements, int num);
	Set(VarType t, uint64_t lowrange, uint64_t highrange);
	~Set();

	MEMALLOC;
private:
	VarType type;
	bool isRange;
	uint64_t low, high;

protected:
	ModelVector<uint64_t> *members;
};
#endif/* SET_H */

