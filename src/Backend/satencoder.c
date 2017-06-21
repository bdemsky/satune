#include "satencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "constraint.h"

SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
	This->varcount=1;
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	ourfree(This);
}

void encodeAllSATEncoder(SATEncoder * This, CSolver *csolver) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		encodeConstraintSATEncoder(This, constraint);
	}
}

Constraint * encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint) {
	switch(GETBOOLEANTYPE(constraint)) {
	case ORDERCONST:
		return encodeOrderSATEncoder(This, (BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder(This, (BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder(This, (BooleanLogic *) constraint);
	}
}

Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint) {
	return NULL;
}

Constraint * getNewVarSATEncoder(SATEncoder *This) {
	Constraint * var=allocVarConstraint(VAR, This->varcount);
	Constraint * varneg=allocVarConstraint(NOTVAR, This->varcount++);
	setNegConstraint(var, varneg);
	setNegConstraint(varneg, var);
	return var;
}

Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint) {
	if (constraint->var == NULL) {
		constraint->var=getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint) {
	/*
	Constraint *left=encodeConstraintSATEncoder(This, constraint->left);
	Constraint *right=NULL;
	if (constraint->right!=NULL)
		right=encodeConstraintSATEncoder(This, constraint->right);
	switch(constraint->op) {
	case L_AND:
		return allocConstraint(AND, left, right);
	case L_OR:
		return allocConstraint(OR, left, right);
	case L_NOT:
		return negateConstraint(allocConstraint(OR, left, NULL));
	case L_XOR: {
		Constraint * nleft=negateConstraint(cloneConstraint(left));
		Constraint * nright=negateConstraint(cloneConstraint(right));
		return allocConstraint(OR,
													 allocConstraint(AND, left, nright),
													 allocConstraint(AND, nleft, right));
	}
	case L_IMPLIES:
		return allocConstraint(IMPLIES, left, right);
		}*/
	return NULL;
}
