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
#include "pass.h"

class Transform : public Pass{
public:
	Transform(CSolver* _solver,Tunables _tunable, TunableDesc* _desc);
	virtual ~Transform();
	virtual bool canExecuteTransform() = 0;
	virtual void doTransform() = 0;
protected:
	// Need solver for translating back the result ...
	CSolver* solver;
};

#endif /* TRANSFORM_H */

