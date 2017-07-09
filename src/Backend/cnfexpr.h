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
typedef struct LitVector LitVector;

VectorDef(LitVector, LitVector *)

struct CNFExpr {
	uint litSize;
	bool isTrue;
	VectorLitVector clauses;
	LitVector singletons;
};

typedef struct CNFExpr CNFExpr;

LitVector * allocLitVector();
void initLitVector(LitVector *This);
void clearLitVector(LitVector *This);
void freeLitVector(LitVector *This);
void deleteLitVector(LitVector *This);
void addLiteralLitVector(LitVector *This, Literal l);
Literal getLiteralLitVector(LitVector *This, uint index);

static inline uint getSizeLitVector(LitVector *This) {return This->size;}

CNFExpr * allocCNFExprBool(bool isTrue);
CNFExpr * allocCNFExprLiteral(Literal l);
void deleteCNFExpr(CNFExpr *This);
void clearCNFExpr(CNFExpr *This, bool isTrue);


bool alwaysTrueCNF(CNFExpr * This);
bool alwaysFalseCNF(CNFExpr * This);
uint getLitSizeCNF(CNFExpr * This);
void clearCNF(CNFExpr *This, bool isTrue);
uint getClauseSizeCNF(CNFExpr * This);
void conjoinCNFLit(CNFExpr *This, Literal l);
void disjoinCNFLit(CNFExpr *This, Literal l);
void disjoinCNFExpr(CNFExpr *This, CNFExpr *expr, bool destroy);
void conjoinCNFExpr(CNFExpr *This, CNFExpr *expr, bool destroy);

#endif
