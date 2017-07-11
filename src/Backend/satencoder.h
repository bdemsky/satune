#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"
#include "structs.h"
#include "inc_solver.h"
#include "constraint.h"

struct SATEncoder {
	uint varcount;
	CNF * cnf;
};

#include "satelemencoder.h"
#include "satorderencoder.h"

SATEncoder * allocSATEncoder();
void deleteSATEncoder(SATEncoder *This);
void encodeAllSATEncoder(CSolver *csolver, SATEncoder *This);
Edge getNewVarSATEncoder(SATEncoder *This);
void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Edge*carray);
Edge encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint);
Edge encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint);
Edge encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint);
Edge encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
void encodeElementSATEncoder(SATEncoder* encoder, Element *This);
Edge encodeElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction *This);
Edge encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Edge encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Edge encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder,ElementFunction* This);
#endif
