#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"

struct SATEncoder {
	uint varcount;
};

SATEncoder * allocSATEncoder();
void deleteSATEncoder(SATEncoder *This);
void encodeAllSATEncoder(SATEncoder *This, CSolver *csolver);
Constraint * getNewVarSATEncoder(SATEncoder *This);
Constraint * encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint);
Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint);
Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint);
Constraint * encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
#endif
