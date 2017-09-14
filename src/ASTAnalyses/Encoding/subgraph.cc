#include "subgraph.h"
#include "encodinggraph.h"
#include "set.h"

EncodingSubGraph::EncodingSubGraph() :
	encodingSize(0),
	numElements(0) {
}

uint hashNodeValuePair(NodeValuePair *nvp) {
	return (uint) (nvp->value ^ ((uintptr_t)nvp->node));
}

bool equalsNodeValuePair(NodeValuePair *nvp1, NodeValuePair *nvp2) {
	return nvp1->value == nvp2->value && nvp1->node == nvp2->node;
}

uint EncodingSubGraph::estimateNewSize(EncodingSubGraph *sg) {
	uint newSize=0;
	SetIteratorEncodingNode * nit = sg->nodes.iterator();
	while(nit->hasNext()) {
		EncodingNode *en = nit->next();
		uint size=estimateNewSize(en);
		if (size > newSize)
			newSize = size;
	}
	delete nit;
	return newSize;
}

uint EncodingSubGraph::estimateNewSize(EncodingNode *n) {
	SetIteratorEncodingEdge * eeit = n->edges.iterator();
	uint newsize=n->getSize();
	while(eeit->hasNext()) {
		EncodingEdge * ee = eeit->next();
		if (ee->left != NULL && ee->left != n && nodes.contains(ee->left)) {
			uint intersectSize = n->s->getUnionSize(ee->left->s);
			if (intersectSize > newsize)
				newsize = intersectSize;
		}
		if (ee->right != NULL && ee->right != n && nodes.contains(ee->right)) {
			uint intersectSize = n->s->getUnionSize(ee->right->s);
			if (intersectSize > newsize)
				newsize = intersectSize;
		}
		if (ee->dst != NULL && ee->dst != n && nodes.contains(ee->dst)) {
			uint intersectSize = n->s->getUnionSize(ee->dst->s);
			if (intersectSize > newsize)
				newsize = intersectSize;
		}
	}
	delete eeit;
	return newsize;
}

void EncodingSubGraph::addNode(EncodingNode *n) {
	nodes.add(n);
	uint newSize=estimateNewSize(n);
	numElements += n->elements.getSize();
	if (newSize > encodingSize)
		encodingSize=newSize;
}

SetIteratorEncodingNode * EncodingSubGraph::nodeIterator() {
	return nodes.iterator();
}

void EncodingSubGraph::encode() {
	SetIteratorEncodingNode *nodeit=nodes.iterator();
	while(nodeit->hasNext()) {
		EncodingNode *node=nodeit->next();
		Set * set = node->s;
		uint setSize = set->getSize();
		for(uint i=0; i<setSize; i++) {
			uint64_t val = set->getElement(i);
			NodeValuePair nvp(node, val);
			if (!map.contains(&nvp)) {
				traverseValue(node, val);
			}
		}
	}
	delete nodeit;
}

void EncodingSubGraph::traverseValue(EncodingNode *node, uint64_t value) {
	
}
