/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   smtratinterpreter.cc
 * Author: hamed
 *
 * Created on February 21, 2019, 2:33 PM
 */

#include "smtratinterpreter.h"

SMTRatInterpreter::SMTRatInterpreter(CSolver *solver) :
	SMTInterpreter(solver)
{
}

void SMTRatInterpreter::compileRunCommand(char *command, size_t size) {
	model_print("Calling SMTRat...\n");
	snprintf(command, size, "./run.sh timeout %u smtrat %s > %s", getTimeout(), SMTFILENAME, SMTSOLUTIONFILE);
}

SMTRatInterpreter::~SMTRatInterpreter() {
}
