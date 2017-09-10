#ifndef ENCODINGGRAPH_H
#define ENCODINGGRAPH_H
#include "classlist.h"
#include "structs.h"

uint hashEncodingEdge(EncodingEdge *edge);
bool equalsEncodingEdge(EncodingEdge *e1, EncodingEdge *e2);

typedef Hashtable<EncodingEdge *, EncodingEdge *, uintptr_t, PTRSHIFT, hashEncodingEdge, equalsEncodingEdge> HashtableEdge;

class EncodingGraph {
 public:
	EncodingGraph(CSolver * solver);
	void buildGraph();
	
	CMEMALLOC;
 private:
	CSolver * solver;
	HashtableEncoding encodingMap;
	HashtableEdge edgeMap;
	HashsetElement discovered;
	void processElement(Element *e);
	void processFunction(ElementFunction *f);
	void processPredicate(BooleanPredicate *b);
	EncodingNode * createNode(Element *e);
	EncodingEdge * getEdge(EncodingNode *left, EncodingNode *right, EncodingNode *dst);
};

class EncodingNode {
 public:
	EncodingNode(Set *_s);
	void addElement(Element *e);
	uint getSize();
	CMEMALLOC;
 private:
	Set *s;
	HashsetElement elements;
	uint numElements;
	friend class EncodingGraph;
};

class EncodingEdge {
 public:
	EncodingEdge(EncodingNode *_l, EncodingNode *_r);
	EncodingEdge(EncodingNode *_l, EncodingNode *_r, EncodingNode *_d);
	CMEMALLOC;
 private:
	EncodingNode *left;
	EncodingNode *right;
	EncodingNode *dst;
	uint numArithOps;
	uint numEquals;
	uint numComparisons;
	friend uint hashEncodingEdge(EncodingEdge *edge);
	friend bool equalsEncodingEdge(EncodingEdge *e1, EncodingEdge *e2);
	friend class EncodingGraph;
};
#endif
