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
	computeEncodingValue();
	computeInequalities();
}

void EncodingSubGraph::computeInequalities() {
	SetIteratorEncodingNode *nodeit=nodes.iterator();
	while(nodeit->hasNext()) {
		EncodingNode *node=nodeit->next();
		SetIteratorEncodingEdge *edgeit=node->edges.iterator();
		while(edgeit->hasNext()) {
			EncodingEdge *edge=edgeit->next();
			if (edge->numComparisons == 0)
				continue;
			if (edge->left == NULL || !nodes.contains(edge->left))
				continue;
			if (edge->right == NULL || !nodes.contains(edge->right))
				continue;
			//examine only once
			if (edge->left != node)
				continue;
			//We have a comparison edge between two nodes in the subgraph
			generateComparison(edge->left, edge->right);
		}
		delete edgeit;
	}
	delete nodeit;
}

void EncodingSubGraph::generateComparison(EncodingNode *left, EncodingNode *right) {
	Set *lset=left->s;
	Set *rset=right->s;
	uint lindex=0, rindex=0;
	uint lSize=lset->getSize(), rSize=rset->getSize();
	uint64_t lVal=lset->getElement(lindex);
	NodeValuePair nvp1(left, lVal);
	EncodingValue *lev = map.get(&nvp1);
	lev->inComparison = true;
	uint64_t rVal=rset->getElement(rindex);
	NodeValuePair nvp2(right, rVal);
	EncodingValue *rev = map.get(&nvp2);
	rev->inComparison = true;
	EncodingValue *last = NULL;

	while(lindex < lSize || rindex < rSize) {
		if (last != NULL) {
			if (lev != NULL)
				last->larger.push(lev);
			if (rev != NULL && lev != rev)
				last->larger.push(rev);
		}
		if (lev != rev) {
			if (rev == NULL || lVal < rVal) {
				if (rev != NULL)
					lev->larger.push(rev);
				last = lev;
				if (++lindex < lSize) {
					lVal=lset->getElement(lindex);
					NodeValuePair nvpl(left, lVal);
					lev = map.get(&nvpl);
					lev->inComparison = true;
				} else
					lev = NULL;
			} else {
				if (lev != NULL)
					rev->larger.push(lev);
				last = rev;
				if (++rindex < rSize) {
					rVal=rset->getElement(rindex);
					NodeValuePair nvpr(right, rVal);
					rev = map.get(&nvpr);
					rev->inComparison = true;
				} else
					rev = NULL;
			}
		} else {
			last = lev;
			if (++lindex < lSize) {
				lVal=lset->getElement(lindex);
				NodeValuePair nvpl(left, lVal);
				lev = map.get(&nvpl);
				lev->inComparison = true;
			} else
				lev = NULL;

			if (++rindex < rSize) {
				rVal=rset->getElement(rindex);
				NodeValuePair nvpr(right, rVal);
				rev = map.get(&nvpr);
				rev->inComparison = true;
			} else
				rev = NULL;
		}
	}
}

void EncodingSubGraph::computeEncodingValue() {
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
	EncodingValue *ecv=new EncodingValue(value);
	HashsetEncodingNode discovered;
	Vector<EncodingNode *> tovisit;
	tovisit.push(node);
	discovered.add(node);
	while(tovisit.getSize()!=0) {
		EncodingNode *n=tovisit.last();tovisit.pop();
		//Add encoding node to structures
		ecv->nodes.add(n);
		NodeValuePair *nvp=new NodeValuePair(n, value);
		map.put(nvp, ecv);
		SetIteratorEncodingEdge *edgeit=node->edges.iterator();
		while(edgeit->hasNext()) {
			EncodingEdge *ee=edgeit->next();
			if (!discovered.contains(ee->left) && nodes.contains(ee->left) && ee->left->s->exists(value)) {
				tovisit.push(ee->left);
				discovered.add(ee->left);
			}
			if (!discovered.contains(ee->right) && nodes.contains(ee->right) && ee->right->s->exists(value)) {
				tovisit.push(ee->right);
				discovered.add(ee->right);
			}
		}
		delete edgeit;
	}
}

