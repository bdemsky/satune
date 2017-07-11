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

SATEncoder * allocSATEncoder();
void deleteSATEncoder(SATEncoder *This);
void encodeAllSATEncoder(CSolver *csolver, SATEncoder *This);
Edge getNewVarSATEncoder(SATEncoder *This);
void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Edge*carray);
Edge encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint);
Edge encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
void createAllTotalOrderConstraintsSATEncoder(SATEncoder* This, Order* order);
Edge getOrderConstraint(HashTableOrderPair *table, OrderPair *pair);
Edge generateTransOrderConstraintSATEncoder(SATEncoder *This, Edge constIJ, Edge constJK, Edge constIK);
Edge encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
Edge encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint);
Edge encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint);
Edge encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint);
Edge encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);

Edge getElementValueBinaryIndexConstraint(SATEncoder * This, Element* element, uint64_t value);
Edge getElementValueConstraint(SATEncoder* encoder, Element* This, uint64_t value);

Edge encodeFunctionElementSATEncoder(SATEncoder* encoder, ElementFunction *This);
Edge encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Edge encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);
Edge encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder,ElementFunction* This);
#endif
