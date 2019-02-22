/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   smtratinterpreter.h
 * Author: hamed
 *
 * Created on February 21, 2019, 2:33 PM
 */

#ifndef SMTRATINTERPRETER_H
#define SMTRATINTERPRETER_H
#include "smtinterpreter.h"

class SMTRatInterpreter: public SMTInterpreter{
public:
	SMTRatInterpreter(CSolver *solver);
	virtual ~SMTRatInterpreter();
protected:
	void compileRunCommand(char * command , size_t size);
};

#endif /* SMTRATINTERPRETER_H */

