/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TunableDependent.cc
 * Author: hamed
 * 
 * Created on October 5, 2018, 11:26 AM
 */

#include "tunabledependent.h"

TunableDependent::TunableDependent(Tunables dependent, Tunables parent):
	dependent(dependent),
	parent(parent)
{
}

TunableDependent::TunableDependent(TunableDependent &setting)
{
	dependent = setting.dependent;
	parent = setting.parent;
}

TunableDependent::~TunableDependent() {
}

