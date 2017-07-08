#include "cnfexpr.h"

#define LITCAPACITY 4
#define MERGESIZE 5

static inline uint boundedSize(uint x) { return (x > MERGESIZE)?MERGESIZE:x; }

LitVector * allocLitVector() {
	LitVector *This=ourmalloc(sizeof(LitVector));
	initLitVector(This);
	return This;
}

void initLitVector(LitVector *This) {
	This->size=0;
	This->capacity=LITCAPACITY;
	This->literals=ourmalloc(This->capacity * sizeof(Literal));
}

void freeLitVector(LitVector *This) {
	ourfree(This->literals);
}

void deleteLitVector(LitVector *This) {
	freeLitVector(This);
	ourfree(This);
}

void addLiteralLitVector(LitVector *This, Literal l) {
	Literal labs = abs(l);
	uint vec_size=This->size;
	uint searchsize=boundedSize(vec_size);
	uint i=0;
	for (; i < searchsize; i++) {
		Literal curr = This->literals[i];
		Literal currabs = abs(curr);
		if (currabs > labs)
			break;
		if (currabs == labs) {
			if (curr == -l)
				This->size = 0; //either true or false now depending on whether this is a conj or disj
			return;
		}
		if ((++This->size) >= This->capacity) {
			This->capacity <<= 1;
			This->literals=ourrealloc(This->literals, This->capacity * sizeof(Literal));
		}
		
		if (vec_size < MERGESIZE) {
			memmove(&This->literals[i+1], &This->literals[i], (vec_size-i) * sizeof(Literal));
			This->literals[i]=l;
		} else {
			This->literals[vec_size]=l;
		}
	}
}

CNFExpr * allocCNFExprBool(bool isTrue) {
	CNFExpr *This=ourmalloc(sizeof(CNFExpr));
	This->litSize=0;
	This->isTrue=isTrue;
	allocInlineVectorLitVector(&This->clauses, 2);
	initLitVector(&This->singletons);
	return This;
}

CNFExpr * allocCNFExprLiteral(Literal l) {
	CNFExpr *This=ourmalloc(sizeof(CNFExpr));
	This->litSize=1;
	This->isTrue=false;
	allocInlineVectorLitVector(&This->clauses, 2);
	initLitVector(&This->singletons);
	addLiteralLitVector(&This->singletons, l);
	return This;
}

void deleteCNFExpr(CNFExpr *This) {
	deleteVectorArrayLitVector(&This->clauses);
	freeLitVector(&This->singletons);
	ourfree(This);
}
