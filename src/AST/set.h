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

struct Set {
	VarType type;
	bool isRange;
	uint64_t low;//also used to count unique items
	uint64_t high;
	VectorInt * members;
};

Set * allocSet(VarType t, uint64_t * elements, uint num);
Set * allocSetRange(VarType t, uint64_t lowrange, uint64_t highrange);
bool existsInSet(Set * This, uint64_t element);
uint getSetSize(Set * This);
void deleteSet(Set * This);
#endif/* SET_H */

