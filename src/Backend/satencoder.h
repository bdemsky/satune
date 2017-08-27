#ifndef SATENCODER_H
#define SATENCODER_H

#include "classlist.h"
#include "structs.h"
#include "inc_solver.h"
#include "constraint.h"
#include "satelemencoder.h"
#include "satorderencoder.h"
#include "satfunctableencoder.h"


class SATEncoder {
 public:
	uint varcount;
	CNF *cnf;
	CSolver *solver;

	SATEncoder(CSolver *solver);
	~SATEncoder();
	void encodeAllSATEncoder(CSolver *csolver);
	Edge encodeConstraintSATEncoder(Boolean *constraint);
	MEMALLOC;
};


	Edge getNewVarSATEncoder(SATEncoder *This);
	void getArrayNewVarsSATEncoder(SATEncoder *encoder, uint num, Edge *carray);

	Edge encodeVarSATEncoder(SATEncoder *This, BooleanVar *constraint);
	Edge encodeLogicSATEncoder(SATEncoder *This, BooleanLogic *constraint);
	Edge encodePredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint);
	Edge encodeTablePredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint);
	void encodeElementSATEncoder(SATEncoder *encoder, Element *This);
	void encodeElementFunctionSATEncoder(SATEncoder *encoder, ElementFunction *This);
	void encodeTableElementFunctionSATEncoder(SATEncoder *encoder, ElementFunction *This);
#endif
