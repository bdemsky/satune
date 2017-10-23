#ifndef ENCODINGGRAPH_H
#define ENCODINGGRAPH_H
#include "classlist.h"
#include "structs.h"
#include "graphstructs.h"

class EncodingGraph {
public:
	EncodingGraph(CSolver *solver);
	~EncodingGraph();
	void buildGraph();
	void encode();

	CMEMALLOC;
private:
	CSolver *solver;
	HashtableEncoding encodingMap;
	HashtableEdge edgeMap;
	Vector<EncodingEdge *> edgeVector;
	HashsetElement discovered;
	HashtableNodeToSubGraph graphMap;
	HashsetEncodingSubGraph subgraphs;

	void encodeParent(Element *e);
	void decideEdges();
	void mergeNodes(EncodingNode *first, EncodingNode *second);
	void processElement(Element *e);
	void processFunction(ElementFunction *f);
	void processPredicate(BooleanPredicate *b);
	EncodingNode *createNode(Element *e);
	EncodingNode *getNode(Element *e);
	EncodingEdge *getEdge(EncodingNode *left, EncodingNode *right, EncodingNode *dst);
	EncodingEdge *createEdge(EncodingNode *left, EncodingNode *right, EncodingNode *dst);
};

class EncodingNode {
public:
	EncodingNode(Set *_s);
	void addElement(Element *e);
	uint getSize() const;
	VarType getType() const;
	void setEncoding(ElementEncodingType e) {encoding = e;}
	ElementEncodingType getEncoding() {return encoding;}
	bool couldBeBinaryIndex() {return encoding == BINARYINDEX || encoding == ELEM_UNASSIGNED;}
	CMEMALLOC;
private:
	Set *s;
	HashsetElement elements;
	HashsetEncodingEdge edges;
	ElementEncodingType encoding;
	friend class EncodingGraph;
	friend class EncodingSubGraph;
};

enum EdgeEncodingType { EDGE_UNASSIGNED, EDGE_BREAK, EDGE_MATCH};
typedef enum EdgeEncodingType EdgeEncodingType;

class EncodingEdge {
public:
	EncodingEdge(EncodingNode *_l, EncodingNode *_r);
	EncodingEdge(EncodingNode *_l, EncodingNode *_r, EncodingNode *_d);
	void setEncoding(EdgeEncodingType e) {encoding = e;}
	uint64_t getValue() const;
	EdgeEncodingType getEncoding() {return encoding;}

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
