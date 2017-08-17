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

LitVector *allocLitVector();
void initLitVector(LitVector *This);
void clearLitVector(LitVector *This);
void freeLitVector(LitVector *This);
LitVector *cloneLitVector(LitVector *orig);
void deleteLitVector(LitVector *This);
void addLiteralLitVector(LitVector *This, Literal l);
Literal getLiteralLitVector(LitVector *This, uint index);
void setLiteralLitVector(LitVector *This, uint index, Literal l);
LitVector *mergeLitVectorLiteral(LitVector *This, Literal l);
LitVector *mergeLitVectors(LitVector *This, LitVector *expr);

static inline uint getSizeLitVector(LitVector *This) {return This->size;}
static inline void setSizeLitVector(LitVector *This, uint size) {This->size = size;}

CNFExpr *allocCNFExprBool(bool isTrue);
CNFExpr *allocCNFExprLiteral(Literal l);
void deleteCNFExpr(CNFExpr *This);
void clearCNFExpr(CNFExpr *This, bool isTrue);
void printCNFExpr(CNFExpr *This);

void copyCNF(CNFExpr *This, CNFExpr *expr, bool destroy);
static inline bool alwaysTrueCNF(CNFExpr *This) {return (This->litSize == 0) && This->isTrue;}
static inline bool alwaysFalseCNF(CNFExpr *This) {return (This->litSize == 0) && !This->isTrue;}
static inline uint getLitSizeCNF(CNFExpr *This) {return This->litSize;}
static inline uint getClauseSizeCNF(CNFExpr *This) {return getSizeLitVector(&This->singletons) + getSizeVectorLitVector(&This->clauses);}
void conjoinCNFLit(CNFExpr *This, Literal l);
void disjoinCNFLit(CNFExpr *This, Literal l);
void disjoinCNFExpr(CNFExpr *This, CNFExpr *expr, bool destroy);
void conjoinCNFExpr(CNFExpr *This, CNFExpr *expr, bool destroy);

#endif
