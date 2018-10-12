/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VarOrderingOpt.cpp
 * Author: hamed
 * 
 * Created on October 11, 2018, 5:31 PM
 */

#include "varorderingopt.h"
#include "csolver.h"
#include "tunable.h"
#include "satencoder.h"
#include "elementencoding.h"
#include "element.h"

VarOrderingOpt::VarOrderingOpt(CSolver *_solver, SATEncoder *_satencoder): Transform(_solver){
	satencoder = _satencoder;
}

VarOrderingOpt::~VarOrderingOpt() {
}

void VarOrderingOpt::doTransform(){
	BooleanVarOrdering direction = (BooleanVarOrdering)solver->getTuner()->getTunable(VARIABLEORDER, &boolVarOrderingDesc);
	if ( direction == CONSTRAINTORDERING ){
		return;
	}
	
	uint size = solver->allElements.getSize();
	if(direction == CHORONOLOGICALORDERING){
		for (uint i = 0; i < size; i++) {
			Element *el = solver->allElements.get(i);
			ElementEncoding *encoding = el->getElementEncoding();
			if (encoding->getElementEncodingType() == ELEM_UNASSIGNED)
				continue;
			satencoder->encodeElementSATEncoder(el);
		}
	}else{
		for (int i = size-1; i>0; i--) {
			Element *el = solver->allElements.get(i);
			ElementEncoding *encoding = el->getElementEncoding();
			if (encoding->getElementEncodingType() == ELEM_UNASSIGNED)
				continue;
			satencoder->encodeElementSATEncoder(el);
		}
	}
}
