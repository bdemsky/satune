#ifndef SUBGRAPH_H
#define SUBGRAPH_H
#include "classlist.h"
#include "structs.h"
#include "graphstructs.h"

class NodeValuePair {
 public:
 NodeValuePair(EncodingNode *n, uint64_t val) : node(n), value(val) {}
	EncodingNode *node;
	uint64_t value;
};

class EncodingValue {
 public:
 EncodingValue(uint64_t _val) : value(_val), inComparison(false) {}
	void merge(EncodingValue *value);
	uint64_t value;
	bool inComparison;
	HashsetEncodingNode nodes;
	Vector<EncodingValue *> larger;
};

uint hashNodeValuePair(NodeValuePair *nvp);
bool equalsNodeValuePair(NodeValuePair *nvp1, NodeValuePair *nvp2);

typedef Hashtable<NodeValuePair *, EncodingValue *, uintptr_t, 4, hashNodeValuePair, equalsNodeValuePair> NVPMap;

class EncodingSubGraph {
 public:
	EncodingSubGraph();
	void addNode(EncodingNode *n);
	SetIteratorEncodingNode * nodeIterator();
	void encode();
	
	CMEMALLOC;
 private:
	uint estimateNewSize(EncodingNode *n);
	uint estimateNewSize(EncodingSubGraph *sg);
	void traverseValue(EncodingNode *node, uint64_t value);
	void computeEncodingValue();
	void computeInequalities();
	void generateComparison(EncodingNode *left, EncodingNode *right);

	HashsetEncodingNode nodes;
	NVPMap map;
	uint encodingSize;
	uint numElements;

	friend class EncodingGraph;
};

#endif
