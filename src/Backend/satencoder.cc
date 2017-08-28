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

//TODO: Should handle sharing of AST Nodes without recoding them a second time

SATEncoder::SATEncoder(CSolver * _solver) :
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
	HSIteratorBoolean *iterator = csolver->getConstraints();
	while (iterator->hasNext()) {
		Boolean *constraint = iterator->next();
		model_print("Encoding All ...\n\n");
		Edge c = encodeConstraintSATEncoder(constraint);
		model_print("Returned Constraint in EncodingAll:\n");
		ASSERT( !equalsEdge(c, E_BOGUS));
		addConstraintCNF(cnf, c);
	}
	delete iterator;
}

Edge SATEncoder::encodeConstraintSATEncoder(Boolean *constraint) {
	switch (GETBOOLEANTYPE(constraint)) {
	case ORDERCONST:
		return encodeOrderSATEncoder((BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder((BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder((BooleanLogic *) constraint);
	case PREDICATEOP:
		return encodePredicateSATEncoder((BooleanPredicate *) constraint);
	default:
		model_print("Unhandled case in encodeConstraintSATEncoder %u", GETBOOLEANTYPE(constraint));
		exit(-1);
	}
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
	case L_AND:
		return constraintAND(cnf, constraint->inputs.getSize(), array);
	case L_OR:
		return constraintOR(cnf, constraint->inputs.getSize(), array);
	case L_NOT:
		return constraintNegate(array[0]);
	case L_XOR:
		return constraintXOR(cnf, array[0], array[1]);
	case L_IMPLIES:
		return constraintIMPLIES(cnf, array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}

Edge SATEncoder::encodePredicateSATEncoder(BooleanPredicate *constraint) {
	switch (GETPREDICATETYPE(constraint->predicate) ) {
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
	switch ( GETELEMENTTYPE(element) ) {
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
	switch (GETFUNCTIONTYPE(function->function)) {
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
