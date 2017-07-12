#ifndef SATFUNCENCODER_H
#define SATFUNCENCODER_H

Edge encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint);
void encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder,ElementFunction* This);
void encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This);

#endif
