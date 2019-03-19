/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TunableDependent.h
 * Author: hamed
 *
 * Created on October 5, 2018, 11:26 AM
 */

#ifndef TUNABLEDEPENDENT_H
#define TUNABLEDEPENDENT_H

#include "tunable.h"

class TunableDependent {
public:
	TunableDependent(Tunables dependent, Tunables parent = (Tunables) - 1);
	TunableDependent(TunableDependent &setting);
	virtual ~TunableDependent();
	Tunables dependent;
	Tunables parent;
};

#endif/* TUNABLEDEPENDENT_H */

