#include "pass.h"
#include "tunable.h"
#include "csolver.h"

Pass::Pass(Tunables _tunable, TunableDesc* _desc): 
	tunable(_tunable),
	tunableDesc(_desc)
{
	
}

Pass::~Pass(){
}
