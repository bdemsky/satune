#ifndef SATORDERENCODER_H
#define SATORDERENCODER_H

Edge encodeOrderSATEncoder(SATEncoder *This, BooleanOrder *constraint);
Edge inferOrderConstraintFromGraph(Order* order, uint64_t _first, uint64_t _second);
Edge getPairConstraint(SATEncoder *This, Order *order, OrderPair *pair);
Edge encodeTotalOrderSATEncoder(SATEncoder *This, BooleanOrder *constraint);
Edge encodePartialOrderSATEncoder(SATEncoder *This, BooleanOrder *constraint);
void createAllTotalOrderConstraintsSATEncoder(SATEncoder *This, Order *order);
Edge getOrderConstraint(HashTableOrderPair *table, OrderPair *pair);
Edge generateTransOrderConstraintSATEncoder(SATEncoder *This, Edge constIJ, Edge constJK, Edge constIK);
#endif
