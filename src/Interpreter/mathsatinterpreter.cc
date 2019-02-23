/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   smtsolvers.cc
 * Author: hamed
 * 
 * Created on February 21, 2019, 12:26 PM
 */

#include "mathsatinterpreter.h"
#include "solver_interface.h"

MathSATInterpreter::MathSATInterpreter(CSolver *solver):
	SMTInterpreter(solver)
{	
}

void MathSATInterpreter::compileRunCommand(char * command , size_t size){
	model_print("Calling MathSAT...\n");
	snprintf(command, size, "./run.sh timeout %u mathsat -model %s > %s", getTimeout(), SMTFILENAME, SMTSOLUTIONFILE);
}

MathSATInterpreter::~MathSATInterpreter(){
}

int MathSATInterpreter::getResult(){
	ifstream input(SMTSOLUTIONFILE, ios::in);
	string line;
	while(getline(input, line)){
		if(line.find("unsat")!= line.npos){
			return IS_UNSAT;
		}
		if(line.find("(") != line.npos){
			char cline [line.size()+1];
			strcpy ( cline, line.c_str() );
			char valuestr [512];
			uint id;
			if (2 == sscanf(cline,"%*[^0123456789]%u %s", &id, valuestr)){
				uint value;
				if (strcmp (valuestr, "true)") == 0 ){
					value =1;
				}else if (strcmp(valuestr, "false)") == 0){
					value = 0;
				}else {
					ASSERT(2 == sscanf(cline, "%*[^0123456789]%u%*[^0123456789]%u", &id, &value));
				}

				model_print("Signature%u = %u\n", id, value);
				sigEnc.setValue(id, value);
			} else {
				ASSERT(0);
			}
		}
	}
	return IS_SAT;
}