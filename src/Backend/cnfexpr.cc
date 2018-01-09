#include "cnfexpr.h"
#include <stdio.h>
#include "common.h"
/*
   V2 Copyright (c) 2014 Ben Chambers, Eugene Goldberg, Pete Manolios,
   Vasilis Papavasileiou, Sudarshan Srinivasan, and Daron Vroon.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.  If
   you download or use the software, send email to Pete Manolios
   (pete@ccs.neu.edu) with your name, contact information, and a short
   note describing what you want to use BAT for.  For any reuse or
   distribution, you must make clear to others the license terms of this
   work.

   Contact Pete Manolios if you want any of these conditions waived.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
   C port of CNF SAT Conversion Copyright Brian Demsky 2017.
 */

#define LITCAPACITY 4
#define MERGESIZE 5

VectorImpl(LitVector, LitVector *, 4)

static inline uint boundedSize(uint x) { return (x > MERGESIZE) ? MERGESIZE : x; }

LitVector *allocLitVector() {
	LitVector *This = (LitVector *)ourmalloc(sizeof(LitVector));
	initLitVector(This);
	return This;
}

void initLitVector(LitVector *This) {
	This->size = 0;
	This->capacity = LITCAPACITY;
	This->literals = (Literal *)ourmalloc(This->capacity * sizeof(Literal));
}

LitVector *cloneLitVector(LitVector *orig) {
	LitVector *This = (LitVector *)ourmalloc(sizeof(LitVector));
	This->size = orig->size;
	This->capacity = orig->capacity;
	This->literals = (Literal *)ourmalloc(This->capacity * sizeof(Literal));
	memcpy(This->literals, orig->literals, sizeof(Literal) * This->size);
	return This;
}

void clearLitVector(LitVector *This) {
	This->size = 0;
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

void setLiteralLitVector(LitVector *This, uint index, Literal l) {
	This->literals[index] = l;
}

void addLiteralLitVector(LitVector *This, Literal l) {
	Literal labs = abs(l);
	uint vec_size = This->size;
	uint searchsize = boundedSize(vec_size);
	uint i = 0;
	for (; i < searchsize; i++) {
		Literal curr = This->literals[i];
		Literal currabs = abs(curr);
		if (currabs > labs)
			break;
		if (currabs == labs) {
			if (curr == -l)
				This->size = 0;	//either true or false now depending on whether this is a conj or disj
			return;
		}
	}
	if ((++This->size) >= This->capacity) {
		This->capacity <<= 1;
		This->literals = (Literal *) ourrealloc(This->literals, This->capacity * sizeof(Literal));
	}

	if (vec_size < MERGESIZE) {
		memmove(&This->literals[i + 1], &This->literals[i], (vec_size - i) * sizeof(Literal));
		This->literals[i] = l;
	} else {
		This->literals[vec_size] = l;
	}
}

CNFExpr *allocCNFExprBool(bool isTrue) {
	CNFExpr *This = (CNFExpr *)ourmalloc(sizeof(CNFExpr));
	This->litSize = 0;
	This->isTrue = isTrue;
	initVectorLitVector(&This->clauses, 2);
	initLitVector(&This->singletons);
	return This;
}

CNFExpr *allocCNFExprLiteral(Literal l) {
	CNFExpr *This = (CNFExpr *)ourmalloc(sizeof(CNFExpr));
	This->litSize = 1;
	This->isTrue = false;
	initVectorLitVector(&This->clauses, 2);
	initLitVector(&This->singletons);
	addLiteralLitVector(&This->singletons, l);
	return This;
}

void clearCNFExpr(CNFExpr *This, bool isTrue) {
	for (uint i = 0; i < getSizeVectorLitVector(&This->clauses); i++) {
		deleteLitVector(getVectorLitVector(&This->clauses, i));
	}
	clearVectorLitVector(&This->clauses);
	clearLitVector(&This->singletons);
	This->litSize = 0;
	This->isTrue = isTrue;
}

void deleteCNFExpr(CNFExpr *This) {
	for (uint i = 0; i < getSizeVectorLitVector(&This->clauses); i++) {
		deleteLitVector(getVectorLitVector(&This->clauses, i));
	}
	deleteVectorArrayLitVector(&This->clauses);
	freeLitVector(&This->singletons);
	ourfree(This);
}

void conjoinCNFLit(CNFExpr *This, Literal l) {
	if (This->litSize == 0 && !This->isTrue)//Handle False
		return;

	This->litSize -= getSizeLitVector(&This->singletons);
	addLiteralLitVector(&This->singletons, l);
	uint newsize = getSizeLitVector(&This->singletons);
	if (newsize == 0)
		clearCNFExpr(This, false);//We found a conflict
	else
		This->litSize += getSizeLitVector(&This->singletons);
}

void copyCNF(CNFExpr *This, CNFExpr *expr, bool destroy) {
	if (destroy) {
		ourfree(This->singletons.literals);
		ourfree(This->clauses.array);
		This->litSize = expr->litSize;
		This->singletons.size = expr->singletons.size;
		This->singletons.literals = expr->singletons.literals;
		This->singletons.capacity = expr->singletons.capacity;
		This->clauses.size = expr->clauses.size;
		This->clauses.array = expr->clauses.array;
		This->clauses.capacity = expr->clauses.capacity;
		ourfree(expr);
	} else {
		for (uint i = 0; i < getSizeLitVector(&expr->singletons); i++) {
			Literal l = getLiteralLitVector(&expr->singletons,i);
			addLiteralLitVector(&This->singletons, l);
		}
		for (uint i = 0; i < getSizeVectorLitVector(&expr->clauses); i++) {
			LitVector *lv = getVectorLitVector(&expr->clauses,i);
			pushVectorLitVector(&This->clauses, cloneLitVector(lv));
		}
		This->litSize = expr->litSize;
	}
}

void conjoinCNFExpr(CNFExpr *This, CNFExpr *expr, bool destroy) {
	if (expr->litSize == 0) {
		if (!expr->isTrue) {
			clearCNFExpr(This, false);
		}
		if (destroy) {
			deleteCNFExpr(expr);
		}
		return;
	}
	if (This->litSize == 0) {
		if (This->isTrue) {
			copyCNF(This, expr, destroy);
		} else if (destroy) {
			deleteCNFExpr(expr);
		}
		return;
	}
	uint litSize = This->litSize;
	litSize -= getSizeLitVector(&expr->singletons);
	for (uint i = 0; i < getSizeLitVector(&expr->singletons); i++) {
		Literal l = getLiteralLitVector(&expr->singletons,i);
		addLiteralLitVector(&This->singletons, l);
		if (getSizeLitVector(&This->singletons) == 0) {
			//Found conflict...
			clearCNFExpr(This, false);
			if (destroy) {
				deleteCNFExpr(expr);
			}
			return;
		}
	}
	litSize += getSizeLitVector(&expr->singletons);
	if (destroy) {
		for (uint i = 0; i < getSizeVectorLitVector(&expr->clauses); i++) {
			LitVector *lv = getVectorLitVector(&expr->clauses,i);
			litSize += getSizeLitVector(lv);
			pushVectorLitVector(&This->clauses, lv);
		}
		clearVectorLitVector(&expr->clauses);
		deleteCNFExpr(expr);
	} else {
		for (uint i = 0; i < getSizeVectorLitVector(&expr->clauses); i++) {
			LitVector *lv = getVectorLitVector(&expr->clauses,i);
			litSize += getSizeLitVector(lv);
			pushVectorLitVector(&This->clauses, cloneLitVector(lv));
		}
	}
	This->litSize = litSize;
}

void disjoinCNFLit(CNFExpr *This, Literal l) {
	if (This->litSize == 0) {
		if (!This->isTrue) {
			This->litSize++;
			addLiteralLitVector(&This->singletons, l);
		}
		return;
	}

	uint litSize = 0;
	uint newindex = 0;
	for (uint i = 0; i < getSizeVectorLitVector(&This->clauses); i++) {
		LitVector *lv = getVectorLitVector(&This->clauses, i);
		addLiteralLitVector(lv, l);
		uint newSize = getSizeLitVector(lv);
		if (newSize != 0) {
			setVectorLitVector(&This->clauses, newindex++, lv);
		} else {
			deleteLitVector(lv);
		}
		litSize += newSize;
	}
	setSizeVectorLitVector(&This->clauses, newindex);

	bool hasSameSingleton = false;
	for (uint i = 0; i < getSizeLitVector(&This->singletons); i++) {
		Literal lsing = getLiteralLitVector(&This->singletons, i);
		if (lsing == l) {
			hasSameSingleton = true;
		} else if (lsing != -l) {
			//Create new LitVector with both l and lsing
			LitVector *newlitvec = allocLitVector();
			addLiteralLitVector(newlitvec, l);
			addLiteralLitVector(newlitvec, lsing);
			litSize += 2;
			pushVectorLitVector(&This->clauses, newlitvec);
		}
	}
	clearLitVector(&This->singletons);
	if (hasSameSingleton) {
		addLiteralLitVector(&This->singletons, l);
		litSize++;
	} else if (litSize == 0) {
		This->isTrue = true;//we are true
	}
	This->litSize = litSize;
}

#define MERGETHRESHOLD 2
LitVector *mergeLitVectors(LitVector *This, LitVector *expr) {
	uint maxsize = This->size + expr->size + MERGETHRESHOLD;
	LitVector *merged = (LitVector *)ourmalloc(sizeof(LitVector));
	merged->literals = (Literal *)ourmalloc(sizeof(Literal) * maxsize);
	merged->capacity = maxsize;
	uint thisSize = boundedSize(This->size);
	uint exprSize = boundedSize(expr->size);
	uint iThis = 0, iExpr = 0, iMerge = 0;
	Literal lThis = This->literals[iThis];
	Literal lExpr = expr->literals[iExpr];
	Literal thisAbs = abs(lThis);
	Literal exprAbs = abs(lExpr);

	while (iThis < thisSize && iExpr < exprSize) {
		if (thisAbs < exprAbs) {
			merged->literals[iMerge++] = lThis;
			lThis = This->literals[++iThis];
			thisAbs = abs(lThis);
		} else if (thisAbs > exprAbs) {
			merged->literals[iMerge++] = lExpr;
			lExpr = expr->literals[++iExpr];
			exprAbs = abs(lExpr);
		} else if (lThis == lExpr) {
			merged->literals[iMerge++] = lExpr;
			lExpr = expr->literals[++iExpr];
			exprAbs = abs(lExpr);
			lThis = This->literals[++iThis];
			thisAbs = abs(lThis);
		} else if (lThis == -lExpr) {
			merged->size = 0;
			return merged;
		}
	}
	if (iThis < thisSize) {
		memcpy(&merged->literals[iMerge], &This->literals[iThis], (thisSize - iThis) * sizeof(Literal));
		iMerge += (thisSize - iThis);
	}
	if (iExpr < exprSize) {
		memcpy(&merged->literals[iMerge], &expr->literals[iExpr], (exprSize - iExpr) * sizeof(Literal));
		iMerge += (exprSize - iExpr);
	}
	merged->size = iMerge;
	return merged;
}

LitVector *mergeLitVectorLiteral(LitVector *This, Literal l) {
	LitVector *copy = cloneLitVector(This);
	addLiteralLitVector(copy, l);
	return copy;
}

void disjoinCNFExpr(CNFExpr *This, CNFExpr *expr, bool destroy) {
	/** Handle the special cases */
	if (expr->litSize == 0) {
		if (expr->isTrue) {
			clearCNFExpr(This, true);
		}
		if (destroy) {
			deleteCNFExpr(expr);
		}
		return;
	} else if (This->litSize == 0) {
		if (!This->isTrue) {
			copyCNF(This, expr, destroy);
		} else if (destroy) {
			deleteCNFExpr(expr);
		}
		return;
	} else if (expr->litSize == 1) {
		disjoinCNFLit(This, getLiteralLitVector(&expr->singletons,0));
		if (destroy) {
			deleteCNFExpr(expr);
		}
		return;
	} else if (destroy && This->litSize == 1) {
		Literal l = getLiteralLitVector(&This->singletons,0);
		copyCNF(This, expr, true);
		disjoinCNFLit(This, l);
		return;
	}

	/** Handle the full cross product */
	uint mergeIndex = 0;
	uint newCapacity = getClauseSizeCNF(This) * getClauseSizeCNF(expr);
	LitVector **mergeArray = (LitVector **)ourmalloc(newCapacity * sizeof(LitVector *));
	uint singleIndex = 0;
	/** First do the singleton, clause pairs */
	for (uint i = 0; i < getSizeLitVector(&This->singletons); i++) {
		Literal lThis = getLiteralLitVector(&This->singletons, i);
		for (uint j = 0; j < getSizeVectorLitVector(&expr->clauses); j++) {
			LitVector *lExpr = getVectorLitVector(&expr->clauses, j);
			LitVector *copy = cloneLitVector(lExpr);
			addLiteralLitVector(copy, lThis);
			if (getSizeLitVector(copy) == 0) {
				deleteLitVector(copy);
			} else {
				mergeArray[mergeIndex++] = copy;
			}
		}
	}

	/** Next do the clause, singleton pairs */
	for (uint i = 0; i < getSizeLitVector(&expr->singletons); i++) {
		Literal lExpr = getLiteralLitVector(&expr->singletons, i);
		for (uint j = 0; j < getSizeVectorLitVector(&This->clauses); j++) {
			LitVector *lThis = getVectorLitVector(&This->clauses, j);
			LitVector *copy = cloneLitVector(lThis);
			addLiteralLitVector(copy, lExpr);
			if (getSizeLitVector(copy) == 0) {
				deleteLitVector(copy);
			} else {
				mergeArray[mergeIndex++] = copy;
			}
		}
	}

	/** Next do the clause, clause pairs */
	for (uint i = 0; i < getSizeVectorLitVector(&This->clauses); i++) {
		LitVector *lThis = getVectorLitVector(&This->clauses, i);
		for (uint j = 0; j < getSizeVectorLitVector(&expr->clauses); j++) {
			LitVector *lExpr = getVectorLitVector(&expr->clauses, j);
			LitVector *merge = mergeLitVectors(lThis, lExpr);
			if (getSizeLitVector(merge) == 0) {
				deleteLitVector(merge);
			} else {
				mergeArray[mergeIndex++] = merge;
			}
		}
		deleteLitVector(lThis);	//Done with this litVector
	}

	/** Finally do the singleton, singleton pairs */
	for (uint i = 0; i < getSizeLitVector(&This->singletons); i++) {
		Literal lThis = getLiteralLitVector(&This->singletons, i);
		for (uint j = 0; j < getSizeLitVector(&expr->singletons); j++) {
			Literal lExpr = getLiteralLitVector(&expr->singletons, j);
			if (lThis == lExpr) {
				//We have a singleton still in the final result
				setLiteralLitVector(&This->singletons, singleIndex++, lThis);
			} else if (lThis != -lExpr) {
				LitVector *mergeLV = allocLitVector();
				addLiteralLitVector(mergeLV, lThis);
				addLiteralLitVector(mergeLV, lExpr);
				mergeArray[mergeIndex++] = mergeLV;
			}
		}
	}

	ourfree(This->clauses.array);
	setSizeLitVector(&This->singletons, singleIndex);
	This->clauses.capacity = newCapacity;
	This->clauses.array = mergeArray;
	This->clauses.size = mergeIndex;
	if (destroy)
		deleteCNFExpr(expr);
}

void printCNFExpr(CNFExpr *This) {
	for (uint i = 0; i < getSizeLitVector(&This->singletons); i++) {
		if (i != 0)
			printf(" ^ ");
		Literal l = getLiteralLitVector(&This->singletons,i);
		printf ("%d",l);
	}
	for (uint i = 0; i < getSizeVectorLitVector(&This->clauses); i++) {
		LitVector *lv = getVectorLitVector(&This->clauses,i);
		printf(" ^ (");
		for (uint j = 0; j < getSizeLitVector(lv); j++) {
			if (j != 0)
				printf(" v ");
			printf("%d", getLiteralLitVector(lv, j));
		}
		printf(")");
	}
}
