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

SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
	This->varcount=1;
	This->cnf=createCNF();
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	deleteCNF(This->cnf);
	ourfree(This);
}

void encodeAllSATEncoder(CSolver *csolver, SATEncoder * This) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		Edge c= encodeConstraintSATEncoder(This, constraint);
		printCNF(c);
		printf("\n\n");
		addConstraintCNF(This->cnf, c);
	}
}

Edge encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint) {
	switch(GETBOOLEANTYPE(constraint)) {
	case ORDERCONST:
		return encodeOrderSATEncoder(This, (BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder(This, (BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder(This, (BooleanLogic *) constraint);
	case PREDICATEOP:
		return encodePredicateSATEncoder(This, (BooleanPredicate *) constraint);
	default:
		model_print("Unhandled case in encodeConstraintSATEncoder %u", GETBOOLEANTYPE(constraint));
		exit(-1);
	}
}

void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Edge * carray) {
	for(uint i=0;i<num;i++)
		carray[i]=getNewVarSATEncoder(encoder);
}

Edge getNewVarSATEncoder(SATEncoder *This) {
	return constraintNewVar(This->cnf);
}

Edge encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint) {
	if (edgeIsNull(constraint->var)) {
		constraint->var=getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Edge encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint) {
	Edge array[getSizeArrayBoolean(&constraint->inputs)];
	for(uint i=0;i<getSizeArrayBoolean(&constraint->inputs);i++)
		array[i]=encodeConstraintSATEncoder(This, getArrayBoolean(&constraint->inputs, i));

	switch(constraint->op) {
	case L_AND:
		return constraintAND(This->cnf, getSizeArrayBoolean(&constraint->inputs), array);
	case L_OR:
		return constraintOR(This->cnf, getSizeArrayBoolean(&constraint->inputs), array);
	case L_NOT:
		ASSERT( getSizeArrayBoolean(&constraint->inputs)==1);
		return constraintNegate(array[0]);
	case L_XOR:
		ASSERT( getSizeArrayBoolean(&constraint->inputs)==2);
		return constraintXOR(This->cnf, array[0], array[1]);
	case L_IMPLIES:
		ASSERT( getSizeArrayBoolean( &constraint->inputs)==2);
		return constraintIMPLIES(This->cnf, array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}

Edge encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	switch(GETPREDICATETYPE(constraint->predicate) ){
		case TABLEPRED:
			return encodeTablePredicateSATEncoder(This, constraint);
		case OPERATORPRED:
			return encodeOperatorPredicateSATEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge encodeTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	switch(constraint->encoding.type){
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

void encodeElementSATEncoder(SATEncoder* encoder, Element *This){
	switch( GETELEMENTTYPE(This) ){
		case ELEMFUNCRETURN:
			addConstraintCNF(encoder->cnf, encodeElementFunctionSATEncoder(encoder, (ElementFunction*) This));
			break;
		case ELEMSET:
			return;
		default:
			ASSERT(0);
	}
}

Edge encodeElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction *This){
	switch(GETFUNCTIONTYPE(This->function)){
		case TABLEFUNC:
			return encodeTableElementFunctionSATEncoder(encoder, This);
		case OPERATORFUNC:
			return encodeOperatorElementFunctionSATEncoder(encoder, This);
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}

Edge encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	switch(getElementFunctionEncoding(This)->type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumTableElemFunctionSATEncoder(encoder, This);
			break;
		case CIRCUIT:
			ASSERT(0);
			break;
		default:
			ASSERT(0);
	}
	return E_BOGUS;
}
