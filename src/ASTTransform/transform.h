/* 
 * File:   transform.h
 * Author: hamed
 *
 * Created on August 26, 2017, 5:13 PM
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "classlist.h"
#include "mymemory.h"
#include "structs.h"

class Transform {
public:
	Transform();
	~Transform();
	void orderIntegerEncodingSATEncoder(CSolver *This, BooleanOrder *boolOrder);
	MEMALLOC;
private:
	HashTableOrderIntegerEncoding* orderIntegerEncoding;
};

#endif /* TRANSFORM_H */

