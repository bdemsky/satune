#ifndef ENCODINGGRAPH_H
#define ENCODINGGRAPH_H
#include "classlist.h"
#include "structs.h"

uint hashEncodingEdge(EncodingEdge *edge);
bool equalsEncodingEdge(EncodingEdge *e1, EncodingEdge *e2);
class EncodingSubGraph;


typedef Hashtable<EncodingEdge *, EncodingEdge *, uintptr_t, PTRSHIFT, hashEncodingEdge, equalsEncodingEdge> HashtableEdge;
typedef Hashset<EncodingNode *, uintptr_t, PTRSHIFT> HashsetEncodingNode;
typedef SetIterator<EncodingNode *, uintptr_t, PTRSHIFT> SetIteratorEncodingNode;
typedef Hashset<EncodingEdge *, uintptr_t, PTRSHIFT> HashsetEncodingEdge;
typedef SetIterator<EncodingEdge *, uintptr_t, PTRSHIFT> SetIteratorEncodingEdge;

typedef Hashtable<EncodingNode *, EncodingSubGraph *, uintptr_t, PTRSHIFT> HashtableNodeToSubGraph;

class EncodingGraph {
 public:
	EncodingGraph(CSolver * solver);
	void buildGraph();
	
	CMEMALLOC;
 private:
	CSolver * solver;
	HashtableEncoding encodingMap;
	HashtableEdge edgeMap;
	Vector<EncodingEdge *> edgeVector;
	HashsetElement discovered;
	HashtableNodeToSubGraph graphMap;

	void decideEdges();
	void mergeNodes(EncodingNode *first, EncodingNode *second);
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
	uint getSize() const;
	VarType getType() const;
	void setEncoding(ElementEncodingType e) {encoding=e;}
	
	CMEMALLOC;
 private:
	Set *s;
	HashsetElement elements;
	HashsetEncodingEdge edges;
	ElementEncodingType encoding;
	friend class EncodingGraph;
	friend class EncodingSubGraph;
};

class EncodingSubGraph {
 public:
	EncodingSubGraph();
	void addNode(EncodingNode *n);
	SetIteratorEncodingNode * nodeIterator();
	
	CMEMALLOC;
 private:
	uint estimateNewSize(EncodingNode *n);
	uint estimateNewSize(EncodingSubGraph *sg);

	HashsetEncodingNode nodes;
	uint encodingSize;
	uint numElements;

	friend class EncodingGraph;
};

enum EdgeEncodingType { EDGE_UNASSIGNED, EDGE_BREAK, EDGE_MATCH};
typedef enum EdgeEncodingType EdgeEncodingType;

class EncodingEdge {
 public:
	EncodingEdge(EncodingNode *_l, EncodingNode *_r);
	EncodingEdge(EncodingNode *_l, EncodingNode *_r, EncodingNode *_d);
	void setEncoding(EdgeEncodingType e) {encoding=e;}
	uint64_t getValue() const;
	CMEMALLOC;
	
 private:
	EncodingNode *left;
	EncodingNode *right;
	EncodingNode *dst;
	EdgeEncodingType encoding;
	uint numArithOps;
	uint numEquals;
	uint numComparisons;
	friend uint hashEncodingEdge(EncodingEdge *edge);
	friend bool equalsEncodingEdge(EncodingEdge *e1, EncodingEdge *e2);
	friend class EncodingGraph;
	friend class EncodingSubGraph;
};

#endif
