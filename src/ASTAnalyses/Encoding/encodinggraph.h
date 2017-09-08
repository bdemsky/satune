#ifndef ENCODINGGRAPH_H
#define ENCODINGGRAPH_H
#include "classlist.h"
#include "structs.h"

class EncodingGraph {
 public:
	EncodingGraph(CSolver * solver);
	EncodingNode * getNode(Element * element);
	void buildGraph();
	
	CMEMALLOC;
 private:
	CSolver * solver;
	HashTableEncoding encodingMap;
	void processElement(Element *e);

};

class EncodingNode {
 public:
	
	CMEMALLOC;
 private:
	
};

class EncodingEdge {
 public:

	CMEMALLOC;
 private:
};

#endif
