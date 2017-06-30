#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"
#include "structs.h"

struct SATEncoder {
	uint varcount;
};

SATEncoder * allocSATEncoder();
void deleteSATEncoder(SATEncoder *This);
void initializeConstraintVars(CSolver* csolver, SATEncoder* This);
void encodeAllSATEncoder(CSolver *csolver, SATEncoder *This);
Constraint * getNewVarSATEncoder(SATEncoder *This);
void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Constraint **carray);
Constraint * encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint);
Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
Constraint * generateTransOrderConstraintSATEncoder(SATEncoder *This, Constraint *first,Constraint *second,Constraint *third);
Constraint * encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
Constraint * encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint);
Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint);
Constraint * encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Constraint * encodeTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Constraint * encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Constraint * encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);

Constraint * getElementValueBinaryIndexConstraint(Element* This, uint64_t value);
Constraint * getElementValueConstraint(Element* This, uint64_t value);

Constraint* encodeFunctionElementSATEncoder(SATEncoder* encoder, ElementFunction *This);
Constraint* encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Constraint* encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Constraint* encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder,ElementFunction* This);
#endif
