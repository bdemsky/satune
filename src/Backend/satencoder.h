#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"
#include "structs.h"
#include "inc_solver.h"
#include "constraint.h"

typedef Hashtable<Boolean *, Node *, uintptr_t, 4> BooleanToEdgeMap;

class SATEncoder {
public:
	int solve(long timeout);
	SATEncoder(CSolver *solver);
	~SATEncoder();
	void resetSATEncoder();
	void encodeAllSATEncoder(CSolver *csolver);
	Edge encodeConstraintSATEncoder(BooleanEdge constraint);
	CNF *getCNF() { return cnf;}
	long long getSolveTime() { return cnf->solveTime; }
	long long getEncodeTime() { return cnf->encodeTime; }
	void freezeElementVariables(ElementEncoding *encoding);

	CMEMALLOC;
private:
	void shouldMemoize(Element *elem, uint64_t val, bool &memo);
	Edge getNewVarSATEncoder();
	void getArrayNewVarsSATEncoder(uint num, Edge *carray);
	Edge encodeVarSATEncoder(BooleanVar *constraint);
	Edge encodeLogicSATEncoder(BooleanLogic *constraint);
	Edge encodePredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeTablePredicateSATEncoder(BooleanPredicate *constraint);
	void encodeElementSATEncoder(Element *element);
	void encodeElementFunctionSATEncoder(ElementFunction *function);
	void encodeTableElementFunctionSATEncoder(ElementFunction *This);
	Edge getElementValueOneHotConstraint(Element *elem, Polarity p, uint64_t value);
	Edge getElementValueUnaryConstraint(Element *elem, Polarity p, uint64_t value);
	Edge getElementValueBinaryIndexConstraint(Element *element, Polarity p, uint64_t value);
	Edge getElementValueBinaryValueConstraint(Element *element, Polarity p, uint64_t value);
	Edge getElementValueConstraint(Element *element, Polarity p, uint64_t value);
	void generateOneHotAtMostOne(ElementEncoding *encoding);
	void generateOneHotBinomialAtMostOne(Edge *array, uint size, uint offset = 0);
	void generateOneHotCommanderAtMostOneRecursive(Edge *array, uint size);
	void generateOneHotSequentialAtMostOne(ElementEncoding *encoding);
	void generateOneHotCommanderEncodingVars(ElementEncoding *encoding);
	void generateOneHotEncodingVars(ElementEncoding *encoding);
	void generateUnaryEncodingVars(ElementEncoding *encoding);
	void generateBinaryIndexEncodingVars(ElementEncoding *encoding);
	void generateBinaryValueEncodingVars(ElementEncoding *encoding);
	void generateElementEncoding(Element *element);
	Edge encodeOperatorPredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeEnumOperatorPredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeUnaryPredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeEnumEqualsPredicateSATEncoder(BooleanPredicate *constraint);
	void encodeOperatorElementFunctionSATEncoder(ElementFunction *This);
	Edge encodeCircuitOperatorPredicateEncoder(BooleanPredicate *constraint);
	Edge encodeOrderSATEncoder(BooleanOrder *constraint);
	Edge inferOrderConstraintFromGraph(Order *order, uint64_t _first, uint64_t _second);
	Edge getPairConstraint(Order *order, OrderPair *pair);
	Edge getPartialPairConstraint(Order *order, OrderPair *pair);
	Edge encodeTotalOrderSATEncoder(BooleanOrder *constraint);
	Edge encodePartialOrderSATEncoder(BooleanOrder *constraint);
	void createAllTotalOrderConstraintsSATEncoder(Order *order);
	void createAllPartialOrderConstraintsSATEncoder(Order *order);
	void createAllTotalOrderConstraintsSATEncoderSparse(Order *order);
	void createAllPartialOrderConstraintsSATEncoderSparse(Order *order);
	Edge getOrderConstraint(HashtableOrderPair *table, OrderPair *pair);
	Edge generateTransOrderConstraintSATEncoder(Edge constIJ, Edge constJK, Edge constIK);
	Edge generatePartialOrderConstraintsSATEncoder(Edge ij,Edge ji, Edge jk, Edge kj,Edge ik, Edge ki);
	Edge encodeEnumEntriesTablePredicateSATEncoder(BooleanPredicate *constraint);
	Edge encodeEnumTablePredicateSATEncoder(BooleanPredicate *constraint);
	void encodeEnumTableElemFunctionSATEncoder(ElementFunction *This);
	void encodeEnumEntriesTableElemFuncSATEncoder(ElementFunction *This);
	void generateAnyValueBinaryIndexEncoding(ElementEncoding *encoding);
	int getMaximumUsedSize(ElementEncoding *encoding);
	void generateAnyValueBinaryIndexEncodingPositive(ElementEncoding *encoding);
	void generateAnyValueBinaryValueEncoding(ElementEncoding *encoding);
	CNF *cnf;
	CSolver *solver;
	BooleanToEdgeMap booledgeMap;
	VectorEdge *vector;
	friend class VarOrderingOpt;
};

void allocElementConstraintVariables(ElementEncoding *ee, uint numVars);
#endif
