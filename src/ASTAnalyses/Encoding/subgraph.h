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

class EncodingValue;

typedef Hashset<EncodingValue *, uintptr_t, PTRSHIFT> HashsetEncodingValue;
typedef SetIterator<EncodingValue *, uintptr_t, PTRSHIFT> SetIteratorEncodingValue;

class EncodingValue {
public:
	EncodingValue(uint64_t _val) : value(_val), encoding(0), inComparison(false), assigned(false) {}
	void merge(EncodingValue *value);
	uint64_t value;
	uint encoding;
	bool inComparison;
	bool assigned;
	HashsetEncodingNode nodes;
	HashsetEncodingValue larger;
	HashsetEncodingValue notequals;
};

uint hashNodeValuePair(NodeValuePair *nvp);
bool equalsNodeValuePair(NodeValuePair *nvp1, NodeValuePair *nvp2);

typedef Hashtable<NodeValuePair *, EncodingValue *, uintptr_t, 0, hashNodeValuePair, equalsNodeValuePair> NVPMap;

class EncodingSubGraph {
public:
	EncodingSubGraph();
	~EncodingSubGraph();
	void addNode(EncodingNode *n);
	SetIteratorEncodingNode *nodeIterator();
	void encode();
	uint getEncoding(EncodingNode *n, uint64_t val);
	uint getEncodingMaxVal(EncodingNode *n) { return maxEncodingVal;}

	CMEMALLOC;
private:
	uint estimateNewSize(EncodingNode *n);
	uint estimateNewSize(EncodingSubGraph *sg);
	void traverseValue(EncodingNode *node, uint64_t value);
	void computeEncodingValue();
	void computeComparisons();
	void computeEqualities();
	void solveComparisons();
	void solveEquals();
	void generateComparison(EncodingNode *left, EncodingNode *right);
	void generateEquals(EncodingNode *left, EncodingNode *right);
	void orderEV(EncodingValue *smaller, EncodingValue *larger);

	HashsetEncodingValue values;
	HashsetEncodingNode nodes;
	NVPMap map;
	uint encodingSize;
	uint numElements;
	uint maxEncodingVal;

	friend class EncodingGraph;
};

#endif
