#ifndef SATFUNCOPENCODER_H
#define SATFUNCOPENCODER_H

Edge encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
void encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder,ElementFunction* This);
Edge encodeCircuitOperatorPredicateEncoder(SATEncoder *This, BooleanPredicate * constraint);
Edge encodeCircuitEquals(SATEncoder * This, BooleanPredicate * constraint);

#endif
