#ifndef CORESTRUCTS_H
#define CORESTRUCTS_H

#include "cppvector.h"
#include "hashset.h"

//How much should we shift pointers when computing hash functions
//5 appears to be the minimum value for Ubuntu 64-bit

#define PTRSHIFT 5

class BooleanEdge {
public:
	BooleanEdge() : b(NULL) {}
	BooleanEdge(Boolean *_b) : b(_b) {}
	BooleanEdge negate() {return BooleanEdge((Boolean *)(((uintptr_t) b) ^ 1));}
	bool operator==(BooleanEdge e) { return b == e.b;}
	bool operator!=(BooleanEdge e) { return b != e.b;}
	bool isNegated() { return ((uintptr_t) b) & 1; }
	Boolean *getBoolean() {return (Boolean *)(((uintptr_t)b) & ~((uintptr_t) 1));}
	Boolean *getRaw() {return b;}
	Boolean *operator->() {return getBoolean();}
	operator bool() {
		return getBoolean() != NULL;
	}
private:
	Boolean *b;
};

inline bool boolEdgeEquals(BooleanEdge b1, BooleanEdge b2) {
	return b1 == b2;
}

inline unsigned int boolEdgeHash(BooleanEdge b) {
	return (unsigned int) (((uintptr_t)b.getRaw()) >> PTRSHIFT);
}

typedef Hashset<BooleanEdge, uintptr_t, PTRSHIFT, &boolEdgeHash, &boolEdgeEquals> HashsetBooleanEdge;
typedef Hashset<Order *, uintptr_t, PTRSHIFT> HashsetOrder;
typedef SetIterator<BooleanEdge, uintptr_t, PTRSHIFT, &boolEdgeHash, &boolEdgeEquals> SetIteratorBooleanEdge;
typedef SetIterator<Order *, uintptr_t, PTRSHIFT> SetIteratorOrder;

#endif
