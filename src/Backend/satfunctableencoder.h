#ifndef SATFUNCTABLEENCODER_H
#define SATFUNCTABLEENCODER_H

Edge encodeEnumEntriesTablePredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint);
Edge encodeEnumTablePredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint);
void encodeEnumTableElemFunctionSATEncoder(SATEncoder *encoder, ElementFunction *This);
void encodeEnumEntriesTableElemFuncSATEncoder(SATEncoder *encoder, ElementFunction *This);

#endif
