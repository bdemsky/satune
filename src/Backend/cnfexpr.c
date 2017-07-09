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

void clearLitVector(LitVector *This) {
	This->size=0;
}

void freeLitVector(LitVector *This) {
	ourfree(This->literals);
}

void deleteLitVector(LitVector *This) {
	freeLitVector(This);
	ourfree(This);
}

Literal getLiteralLitVector(LitVector *This, uint index) {
	return This->literals[index];
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

void clearCNFExpr(CNFExpr *This, bool isTrue) {
	for(uint i=0;i<getSizeVectorLitVector(&This->clauses);i++) {
		deleteLitVector(getVectorLitVector(&This->clauses, i));
	}
	clearVectorLitVector(&This->clauses);
	clearLitVector(&This->singletons);
	This->litSize=0;
	This->isTrue=isTrue;
}

void deleteCNFExpr(CNFExpr *This) {
	for(uint i=0;i<getSizeVectorLitVector(&This->clauses);i++) {
		deleteLitVector(getVectorLitVector(&This->clauses, i));
	}
	deleteVectorArrayLitVector(&This->clauses);
	freeLitVector(&This->singletons);
	ourfree(This);
}

void conjoinCNFLit(CNFExpr *This, Literal l) {
	if (This->litSize==0 && !This->isTrue) //Handle False
		return;
	
	This->litSize-=getSizeLitVector(&This->singletons);
	addLiteralLitVector(&This->singletons, l);
	uint newsize=getSizeLitVector(&This->singletons);
	if (newsize==0)
		clearCNF(This, false); //We found a conflict
	else
		This->litSize+=getSizeLitVector(&This->singletons);
}

void disjoinCNFLit(CNFExpr *This, Literal l) {
	if (This->litSize==0) {
		if (!This->isTrue) {
			This->litSize++;
			addLiteralLitVector(&This->singletons, l);
		}
		return;
	}
	uint litSize=0;
	uint newindex=0;
	for(uint i=0;i<getSizeVectorLitVector(&This->clauses);i++) {
		LitVector * lv=getVectorLitVector(&This->clauses, i);
		addLiteralLitVector(lv, l);
		uint newSize=getSizeLitVector(lv);
		if (newSize!=0) {
			setVectorLitVector(&This->clauses, newindex++, lv);
		} else {
			deleteLitVector(lv);
		}
		litSize+=newSize;
	}
	setSizeVectorLitVector(&This->clauses, newindex);

	bool hasSameSingleton=false;
	for(uint i=0;i<getSizeLitVector(&This->singletons);i++) {
		Literal lsing=getLiteralLitVector(&This->singletons, i);
		if (lsing == l) {
			hasSameSingleton=true;
		} else if (lsing != -l) {
			//Create new LitVector with both l and lsing
			LitVector *newlitvec=allocLitVector();
			addLiteralLitVector(newlitvec, l);
			addLiteralLitVector(newlitvec, lsing);
			litSize+=2;
			pushVectorLitVector(&This->clauses, newlitvec);
		}
	}
	clearLitVector(&This->singletons);
	if (hasSameSingleton) {
		addLiteralLitVector(&This->singletons, l);
		litSize++;
	}
	This->litSize=litSize;
}
