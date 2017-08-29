/* 
 * File:   pass.h
 * Author: hamed
 *
 * Created on August 28, 2017, 6:23 PM
 */

#ifndef PASS_H
#define PASS_H
#include "classlist.h"
#include "mymemory.h"
#include "structs.h"
#include "tunable.h"
#include "csolver.h"

class Pass{
public:
	Pass(){};
	virtual ~Pass(){};
	virtual inline bool canExecutePass(CSolver* This, uint type, Tunables tunable, TunableDesc* desc){
		return GETVARTUNABLE(This->getTuner(), type, tunable, desc);
	}
	MEMALLOC;

};


#endif /* PASS_H */

