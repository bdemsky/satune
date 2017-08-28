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
#include "satfuncopencoder.h"

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
		return encodeOrderSATEncoder(this, (BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder(this, (BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder(this, (BooleanLogic *) constraint);
	case PREDICATEOP:
		return encodePredicateSATEncoder(this, (BooleanPredicate *) constraint);
	default:
		model_print("Unhandled case in encodeConstraintSATEncoder %u", GETBOOLEANTYPE(constraint));
		exit(-1);
	}
}

void getArrayNewVarsSATEncoder(SATEncoder *encoder, uint num, Edge *carray) {
	for (uint i = 0; i < num; i++)
		carray[i] = getNewVarSATEncoder(encoder);
}

Edge getNewVarSATEncoder(SATEncoder *This) {
	return constraintNewVar(This->getCNF());
}

Edge encodeVarSATEncoder(SATEncoder *This, BooleanVar *constraint) {
	if (edgeIsNull(constraint->var)) {
		constraint->var = getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Edge encodeLogicSATEncoder(SATEncoder *This, BooleanLogic *constraint) {
	Edge array[constraint->inputs.getSize()];
	for (uint i = 0; i < constraint->inputs.getSize(); i++)
		array[i] = This->encodeConstraintSATEncoder(constraint->inputs.get(i));

	switch (constraint->op) {
	case L_AND:
		return constraintAND(This->getCNF(), constraint->inputs.getSize(), array);
	case L_OR:
		return constraintOR(This->getCNF(), constraint->inputs.getSize(), array);
	case L_NOT:
		return constraintNegate(array[0]);
	case L_XOR:
		return constraintXOR(This->getCNF(), array[0], array[1]);
	case L_IMPLIES:
		return constraintIMPLIES(This->getCNF(), array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}

Edge encodePredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint) {
	switch (GETPREDICATETYPE(constraint->predicate) ) {
	case TABLEPRED:
		return encodeTablePredicateSATEncoder(This, constraint);
	case OPERATORPRED:
		return encodeOperatorPredicateSATEncoder(This, constraint);
	default:
		ASSERT(0);
	}
	return E_BOGUS;
}

Edge encodeTablePredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint) {
	switch (constraint->encoding.type) {
	case ENUMERATEIMPLICATIONS:
	case ENUMERATEIMPLICATIONSNEGATE:
		return encodeEnumTablePredicateSATEncoder(This, constraint);
	case CIRCUIT:
		ASSERT(0);
		break;
	default:
		ASSERT(0);
	}
	return E_BOGUS;
}

void encodeElementSATEncoder(SATEncoder *encoder, Element *This) {
	switch ( GETELEMENTTYPE(This) ) {
	case ELEMFUNCRETURN:
		generateElementEncoding(encoder, This);
		encodeElementFunctionSATEncoder(encoder, (ElementFunction *) This);
		break;
	case ELEMSET:
		generateElementEncoding(encoder, This);
		return;
	case ELEMCONST:
		return;
	default:
		ASSERT(0);
	}
}

void encodeElementFunctionSATEncoder(SATEncoder *encoder, ElementFunction *This) {
	switch (GETFUNCTIONTYPE(This->function)) {
	case TABLEFUNC:
		encodeTableElementFunctionSATEncoder(encoder, This);
		break;
	case OPERATORFUNC:
		encodeOperatorElementFunctionSATEncoder(encoder, This);
		break;
	default:
		ASSERT(0);
	}
}

void encodeTableElementFunctionSATEncoder(SATEncoder *encoder, ElementFunction *This) {
	switch (getElementFunctionEncoding(This)->type) {
	case ENUMERATEIMPLICATIONS:
		encodeEnumTableElemFunctionSATEncoder(encoder, This);
		break;
	case CIRCUIT:
		ASSERT(0);
		break;
	default:
		ASSERT(0);
	}
}
