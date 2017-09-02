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
	Transform(CSolver *_solver);
	virtual ~Transform();
	virtual void doTransform() = 0;
	CMEMALLOC;
protected:
	// Need solver for translating back the result ...
	CSolver *solver;
};

#endif/* TRANSFORM_H */

