#include "satencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "constraint.h"
#include "common.h"
#include "element.h"
#include "function.h"
#include "tableentry.h"
#include "table.h"
#include "order.h"
#include "predicate.h"
#include "set.h"

SATEncoder::SATEncoder(CSolver *_solver) :
	cnf(createCNF()),
	solver(_solver) {
}

SATEncoder::~SATEncoder() {
	deleteCNF(cnf);
}

int SATEncoder::solve() {
	return solveCNF(cnf);
}

void SATEncoder::encodeAllSATEncoder(CSolver *csolver) {
	SetIteratorBoolean *iterator = csolver->getConstraints();
	while (iterator->hasNext()) {
		Boolean *constraint = iterator->next();
		Edge c = encodeConstraintSATEncoder(constraint);
		addConstraintCNF(cnf, c);
	}
	delete iterator;
}

Edge SATEncoder::encodeConstraintSATEncoder(Boolean *constraint) {
	Edge result;
	if (booledgeMap.contains(constraint)) {
		Edge e = {(Node *) booledgeMap.get(constraint)};
		return e;
	}

	switch (constraint->type) {
	case ORDERCONST:
		result = encodeOrderSATEncoder((BooleanOrder *) constraint);
		break;
	case BOOLEANVAR:
		result = encodeVarSATEncoder((BooleanVar *) constraint);
		break;
	case LOGICOP:
		result = encodeLogicSATEncoder((BooleanLogic *) constraint);
		break;
	case PREDICATEOP:
		result = encodePredicateSATEncoder((BooleanPredicate *) constraint);
		break;
	default:
		model_print("Unhandled case in encodeConstraintSATEncoder %u", constraint->type);
		exit(-1);
	}
	booledgeMap.put(constraint, result.node_ptr);
	return result;
}

void SATEncoder::getArrayNewVarsSATEncoder(uint num, Edge *carray) {
	for (uint i = 0; i < num; i++)
		carray[i] = getNewVarSATEncoder();
}

Edge SATEncoder::getNewVarSATEncoder() {
	return constraintNewVar(cnf);
}

Edge SATEncoder::encodeVarSATEncoder(BooleanVar *constraint) {
	if (edgeIsNull(constraint->var)) {
		constraint->var = getNewVarSATEncoder();
	}
	return constraint->var;
}

Edge SATEncoder::encodeLogicSATEncoder(BooleanLogic *constraint) {
	Edge array[constraint->inputs.getSize()];
	for (uint i = 0; i < constraint->inputs.getSize(); i++)
		array[i] = encodeConstraintSATEncoder(constraint->inputs.get(i));

	switch (constraint->op) {
	case SATC_AND:
		return constraintAND(cnf, constraint->inputs.getSize(), array);
	case SATC_OR:
		return constraintOR(cnf, constraint->inputs.getSize(), array);
	case SATC_NOT:
		return constraintNegate(array[0]);
	case SATC_XOR:
		return constraintXOR(cnf, array[0], array[1]);
	case SATC_IFF:
		return constraintIFF(cnf, array[0], array[1]);
	case SATC_IMPLIES:
		return constraintIMPLIES(cnf, array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}

Edge SATEncoder::encodePredicateSATEncoder(BooleanPredicate *constraint) {
	switch (constraint->predicate->type) {
	case TABLEPRED:
		return encodeTablePredicateSATEncoder(constraint);
	case OPERATORPRED:
		return encodeOperatorPredicateSATEncoder(constraint);
	default:
		ASSERT(0);
	}
	return E_BOGUS;
}

Edge SATEncoder::encodeTablePredicateSATEncoder(BooleanPredicate *constraint) {
	switch (constraint->encoding.type) {
	case ENUMERATEIMPLICATIONS:
	case ENUMERATEIMPLICATIONSNEGATE:
		return encodeEnumTablePredicateSATEncoder(constraint);
	case CIRCUIT:
		ASSERT(0);
		break;
	default:
		ASSERT(0);
	}
	return E_BOGUS;
}

void SATEncoder::encodeElementSATEncoder(Element *element) {
	/* Check to see if we have already encoded this element. */
	if (getElementEncoding(element)->variables != NULL)
		return;

	/* Need to encode. */
	switch ( element->type) {
	case ELEMFUNCRETURN:
		generateElementEncoding(element);
		encodeElementFunctionSATEncoder((ElementFunction *) element);
		break;
	case ELEMSET:
		generateElementEncoding(element);
		return;
	case ELEMCONST:
		return;
	default:
		ASSERT(0);
	}
}

void SATEncoder::encodeElementFunctionSATEncoder(ElementFunction *function) {
	switch (function->function->type) {
	case TABLEFUNC:
		encodeTableElementFunctionSATEncoder(function);
		break;
	case OPERATORFUNC:
		encodeOperatorElementFunctionSATEncoder(function);
		break;
	default:
		ASSERT(0);
	}
}

void SATEncoder::encodeTableElementFunctionSATEncoder(ElementFunction *function) {
	switch (getElementFunctionEncoding(function)->type) {
	case ENUMERATEIMPLICATIONS:
		encodeEnumTableElemFunctionSATEncoder(function);
		break;
	case CIRCUIT:
		ASSERT(0);
		break;
	default:
		ASSERT(0);
	}
}
