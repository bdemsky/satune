#ifndef ITERATOR_H
#define ITERATOR_H
#include "classlist.h"
#include "structs.h"

class BooleanIterator {
 public:
	BooleanIterator(CSolver * _solver);
	~BooleanIterator();
	bool hasNext();
	Boolean * next();
	CMEMALLOC;
 private:
	SetIteratorBooleanEdge * solverit;
	HashsetBoolean discovered;
	Vector<Boolean *> boolean;
	Vector<uint> index;
	void updateNext();
};

class ElementIterator {
 public:
	CMEMALLOC;
 private:
	BooleanIterator bit;
};


#endif
