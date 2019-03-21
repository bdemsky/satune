/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   mathsatinterpreter.h
 * Author: hamed
 *
 * Created on February 21, 2019, 12:26 PM
 */

#ifndef MATHSATINTERPRETER_H
#define MATHSATINTERPRETER_H

#include "smtinterpreter.h"


class MathSATInterpreter : public SMTInterpreter {
public:
	MathSATInterpreter(CSolver *solver);
	virtual ~MathSATInterpreter();
protected:
	virtual void compileRunCommand(char *command, size_t size);
	virtual int getResult();
};


#endif/* SMTSOLVERS_H */

