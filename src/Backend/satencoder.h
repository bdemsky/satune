#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"
#include "structs.h"
#include "inc_solver.h"
#include "constraint.h"

typedef HashTable<Boolean *, Node *, uintptr_t, 4> BooleanToEdgeMap;

class SATEncoder {
 public:
	int solve();
	SATEncoder(CSolver *solver);
	~SATEncoder();
	void encodeAllSATEncoder(CSolver *csolver);
	Edge encodeConstraintSATEncoder(Boolean *constraint);
	CNF * getCNF() { return cnf;}
	long long getSolveTime() { return cnf->solveTime; }
	long long getEncodeTime() { return cnf->encodeTime; }
	
	MEMALLOC;
 private:
	Edge getNewVarSATEncoder();
	void getArrayNewVarsSATEncoder(uint num, Edge *carray);
	Edge encodeVarSATEncoder(BooleanVar *constraint);
	Edge encodeLogicSATEncoder(BooleanLogic *constraint);
	Edge encodePredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeTablePredicateSATEncoder(BooleanPredicate *constraint);
	void encodeElementSATEncoder(Element *element);
	void encodeElementFunctionSATEncoder(ElementFunction *function);
	void encodeTableElementFunctionSATEncoder(ElementFunction *This);
	Edge getElementValueOneHotConstraint(Element *elem, uint64_t value);
	Edge getElementValueUnaryConstraint(Element *elem, uint64_t value);
	Edge getElementValueBinaryIndexConstraint(Element *element, uint64_t value);
	Edge getElementValueBinaryValueConstraint(Element *element, uint64_t value);
	Edge getElementValueConstraint(Element *element, uint64_t value);
	void generateOneHotEncodingVars(ElementEncoding *encoding);
	void generateUnaryEncodingVars(ElementEncoding *encoding);
	void generateBinaryIndexEncodingVars(ElementEncoding *encoding);
	void generateBinaryValueEncodingVars(ElementEncoding *encoding);
	void generateElementEncoding(Element *element);
	Edge encodeOperatorPredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeEnumOperatorPredicateSATEncoder(BooleanPredicate *constraint);
	void encodeOperatorElementFunctionSATEncoder(ElementFunction *This);
	Edge encodeCircuitOperatorPredicateEncoder(BooleanPredicate *constraint);
	Edge encodeOrderSATEncoder(BooleanOrder *constraint);
	Edge inferOrderConstraintFromGraph(Order *order, uint64_t _first, uint64_t _second);
	Edge getPairConstraint(Order *order, OrderPair *pair);
	Edge encodeTotalOrderSATEncoder(BooleanOrder *constraint);
	Edge encodePartialOrderSATEncoder(BooleanOrder *constraint);
	void createAllTotalOrderConstraintsSATEncoder(Order *order);
	Edge getOrderConstraint(HashTableOrderPair *table, OrderPair *pair);
	Edge generateTransOrderConstraintSATEncoder(Edge constIJ, Edge constJK, Edge constIK);
	Edge encodeEnumEntriesTablePredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeEnumTablePredicateSATEncoder(BooleanPredicate *constraint);
	void encodeEnumTableElemFunctionSATEncoder(ElementFunction *This);
	void encodeEnumEntriesTableElemFuncSATEncoder(ElementFunction *This);
	
	CNF *cnf;
	CSolver *solver;
	BooleanToEdgeMap booledgeMap;

};

void allocElementConstraintVariables(ElementEncoding *ee, uint numVars);
Edge getOrderConstraint(HashTableOrderPair *table, OrderPair *pair);
#endif
