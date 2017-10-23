#ifndef ITERATOR_H
#define ITERATOR_H
#include "classlist.h"
#include "structs.h"

class BooleanIterator {
public:
	BooleanIterator(CSolver *_solver);
	~BooleanIterator();
	bool hasNext();
	Boolean *next();
	CMEMALLOC;

private:
	SetIteratorBooleanEdge *solverit;
	HashsetBoolean discovered;
	Vector<Boolean *> boolean;
	Vector<uint> index;
	void updateNext();
};

class ElementIterator {
public:
	ElementIterator(CSolver *_solver);
	~ElementIterator();
	bool hasNext();
	Element *next();
	CMEMALLOC;

private:
	BooleanIterator bit;
	BooleanPredicate *base;
	uint baseindex;

	HashsetElement discovered;

	Vector<Element *> element;
	Vector<uint> index;
	void updateNext();
};

#endif
