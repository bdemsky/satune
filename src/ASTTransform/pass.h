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
	Pass(Tunables _tunable, TunableDesc* _desc);
	virtual ~Pass();
	virtual inline bool canExecutePass(CSolver* This, uint type=0){
		return GETVARTUNABLE(This->getTuner(), type, tunable, tunableDesc);
	}
	MEMALLOC;
protected:
	Tunables tunable;
	TunableDesc* tunableDesc;
};


#endif /* PASS_H */

