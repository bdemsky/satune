#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"
#include "structs.h"

struct SATEncoder {
	uint varcount;
	//regarding managing memory 
	VectorConstraint vars;
	VectorConstraint allConstraints;
};

inline VectorConstraint* getSATEncoderVars(SATEncoder* ne){
	return &ne->vars;
}
inline VectorConstraint* getSATEncoderAllConstraints(SATEncoder* ne){
	return &ne->allConstraints;
}
SATEncoder * allocSATEncoder();
void deleteSATEncoder(SATEncoder *This);
void encodeAllSATEncoder(SATEncoder *This, CSolver *csolver);
Constraint * getNewVarSATEncoder(SATEncoder *This);
void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Constraint **carray);
Constraint * encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint);
Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint);
Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint);
Constraint * encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);

Constraint* encodeFunctionElementSATEncoder(SATEncoder* encoder, ElementFunction *This);
Constraint* encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Constraint* encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Constraint* encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder,ElementFunction* This);
#endif
