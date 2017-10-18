#include "orderpair.h"
#include "constraint.h"
#include "csolver.h"
#include "satencoder.h"

OrderPair::OrderPair(uint64_t _first, uint64_t _second, Edge _constraint) :
	first(_first),
	second(_second),
	constraint(_constraint) {
}

OrderPair::OrderPair() :
	first(0),
	second(0),
	constraint(E_NULL) {
}

OrderPair::~OrderPair() {
}

Edge OrderPair::getConstraint() {
	return constraint;
}

Edge OrderPair::getNegatedConstraint() {
	return constraintNegate(constraint);
}

bool OrderPair::getConstraintValue(CSolver *solver) {
	return getValueCNF(solver->getSATEncoder()->getCNF(), constraint);
}

bool OrderPair::getNegatedConstraintValue(CSolver *solver) {
	return getValueCNF(solver->getSATEncoder()->getCNF(), constraintNegate(constraint));
}
