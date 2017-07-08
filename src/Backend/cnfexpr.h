#ifndef CNFEXPR_H
#define CNFEXPR_H
#include "classlist.h"
#include "vector.h"

typedef int Literal;


struct LitVector {
	uint size;
	uint capacity;
	Literal *literals;
};
typedef CNFClause CNFClause;

VectorDef(LitVector, LitVector *)

struct CNFExpr {
	uint litSize;
	bool isTrue;
	VectorLitVector clauses;
	LitVector singletons;
};

typedef CNFExpr CNFExpr;

bool alwaysTrueCNF(CNFExpr * This);
bool alwaysFalseCNF(CNFExpr * This);
uint getLitSizeCNF(CNFExpr * This);
void clearCNF(CNFExpr *This, bool isTrue);
uint getClauseSizeCNF(CNFExpr * This);
#endif
