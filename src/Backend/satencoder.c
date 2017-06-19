#include "satencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "constraint.h"

SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
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
	switch(constraint->btype) {
	case ORDERCONST:
		return encodeOrderSATEncoder(This, (BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder(This, (BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder(This, (BooleanLogic *) constraint);
	case COMPARE:
		return encodeCompareSATEncoder(This, (BooleanComp *) constraint);
	}
}

Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint) {
	return NULL;
}

Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint) {
	return NULL;
}

Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint) {
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
	}
	return NULL;
}

Constraint * encodeCompareSATEncoder(SATEncoder *This, BooleanComp * constraint) {
	return NULL;
}
