/* 
 * File:   transform.cc
 * Author: hamed
 * 
 * Created on August 26, 2017, 5:14 PM
 */

#include "transform.h"

Transform::Transform(CSolver* _solver, Tunables _tunable, TunableDesc* _desc):
	Pass(_tunable, _desc)
{
	solver = _solver;
}

Transform::~Transform(){
}
