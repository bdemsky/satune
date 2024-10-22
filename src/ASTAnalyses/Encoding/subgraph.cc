#include "subgraph.h"
#include "encodinggraph.h"
#include "set.h"
#include "qsort.h"

EncodingSubGraph::EncodingSubGraph() :
	maxEncodingVal(0) {
}

EncodingSubGraph::~EncodingSubGraph() {
	map.resetAndDeleteKeys();
	values.resetAndDelete();
}

uint hashNodeValuePair(NodeValuePair *nvp) {
	return (uint) (nvp->value ^ ((uintptr_t)nvp->node));
}

bool equalsNodeValuePair(NodeValuePair *nvp1, NodeValuePair *nvp2) {
	return nvp1->value == nvp2->value && nvp1->node == nvp2->node;
}

int sortEncodingValue(const void *p1, const void *p2) {
	const EncodingValue *e1 = *(const EncodingValue **) p1;
	const EncodingValue *e2 = *(const EncodingValue **) p2;
	uint se1 = e1->notequals.getSize();
	uint se2 = e2->notequals.getSize();
	if (se1 > se2)
		return -1;
	else if (se2 == se1)
		return 0;
	else
		return 1;
}

uint EncodingSubGraph::getEncoding(EncodingNode *n, uint64_t val) {
	NodeValuePair nvp(n, val);
	EncodingValue *ev = map.get(&nvp);
	return ev->encoding;
}

void EncodingSubGraph::solveEquals() {
	Vector<EncodingValue *> toEncode;
	Vector<bool> encodingArray;
	SetIteratorEncodingValue *valIt = values.iterator();
	while (valIt->hasNext()) {
		EncodingValue *ev = valIt->next();
		if (!ev->inComparison)
			toEncode.push(ev);
		else
			ev->assigned = true;
	}
	delete valIt;
	bsdqsort(toEncode.expose(), toEncode.getSize(), sizeof(EncodingValue *), sortEncodingValue);
	uint toEncodeSize = toEncode.getSize();
	for (uint i = 0; i < toEncodeSize; i++) {
		EncodingValue *ev = toEncode.get(i);
		encodingArray.clear();
		SetIteratorEncodingValue *conflictIt = ev->notequals.iterator();
		while (conflictIt->hasNext()) {
			EncodingValue *conflict = conflictIt->next();
			if (conflict->assigned) {
				encodingArray.setExpand(conflict->encoding, true);
			}
		}
		delete conflictIt;
		uint encoding = 0;
		for (; encoding < encodingArray.getSize(); encoding++) {
			//See if this is unassigned
			if (!encodingArray.get(encoding))
				break;
		}
		if (encoding > maxEncodingVal)
			maxEncodingVal = encoding;
		ev->encoding = encoding;
		ev->assigned = true;
	}
}

void EncodingSubGraph::solveComparisons() {
	HashsetEncodingValue discovered;
	Vector<EncodingValue *> tovisit;
	SetIteratorEncodingValue *valIt = values.iterator();
	while (valIt->hasNext()) {
		EncodingValue *ev = valIt->next();
		if (discovered.add(ev)) {
			tovisit.push(ev);
			while (tovisit.getSize() != 0) {
				EncodingValue *val = tovisit.last(); tovisit.pop();
				SetIteratorEncodingValue *nextIt = val->larger.iterator();
				uint minVal = val->encoding + 1;
				while (nextIt->hasNext()) {
					EncodingValue *nextVal = nextIt->next();
					if (nextVal->encoding < minVal) {
						if (minVal > maxEncodingVal)
							maxEncodingVal = minVal;
						nextVal->encoding = minVal;
						discovered.add(nextVal);
						tovisit.push(nextVal);
					}
				}
				delete nextIt;
			}
		}
	}
	delete valIt;
}

uint EncodingSubGraph::estimateNewSize(EncodingSubGraph *sg) {
	uint newSize = sg->allValues.getSize() + allValues.getSize();
	SetIterator64Int *it = sg->allValues.iterator();

	while (it->hasNext()) {
		uint64_t val = it->next();
		if (allValues.contains(val))
			newSize--;
	}
	delete it;
	return newSize;
}

double EncodingSubGraph::measureSimilarity(EncodingNode *node) {
	uint common = 0;
	uint size = node->getSize();
	for (uint i = 0; i < size; i++) {
		uint64_t val = node->getIndex(i);
		if (allValues.contains(val))
			common++;
	}
	return common * 1.0 / allValues.getSize() + common * 1.0 / node->getSize();
}

double EncodingSubGraph::measureSimilarity(EncodingSubGraph *sg) {
	uint common = 0;
	SetIterator64Int *setIter1 = allValues.iterator();
	while (setIter1->hasNext()) {
		uint64_t item1 = setIter1->next();
		if ( sg->allValues.contains(item1)) {
			common++;
		}
	}
	delete setIter1;
	return common * 1.0 / allValues.getSize() + common * 1.0 / sg->allValues.getSize();
}

uint EncodingSubGraph::estimateNewSize(EncodingNode *n) {
	uint nSize = n->getSize();
	uint newSize = allValues.getSize() + nSize;
	for (uint i = 0; i < nSize; i++) {
		if (allValues.contains(n->getIndex(i)))
			newSize--;
	}
	return newSize;
}

uint EncodingSubGraph::numValues() {
	return allValues.getSize();
}

void EncodingSubGraph::addNode(EncodingNode *n) {
	nodes.add(n);
	uint size = n->getSize();
	for (uint i = 0; i < size; i++)
		allValues.add(n->getIndex(i));
}

SetIteratorEncodingNode *EncodingSubGraph::nodeIterator() {
	return nodes.iterator();
}

void EncodingSubGraph::encode() {
	computeEncodingValue();
	computeComparisons();
	computeEqualities();
	solveComparisons();
	solveEquals();
}

void EncodingSubGraph::computeEqualities() {
	SetIteratorEncodingNode *nodeit = nodes.iterator();
	while (nodeit->hasNext()) {
		EncodingNode *node = nodeit->next();
		generateEquals(node, node);

		SetIteratorEncodingEdge *edgeit = node->edges.iterator();
		while (edgeit->hasNext()) {
			EncodingEdge *edge = edgeit->next();
			//skip over comparisons as we have already handled them
			if (edge->numComparisons != 0)
				continue;
			if (edge->numEquals == 0)
				continue;
			if (edge->left == NULL || !nodes.contains(edge->left))
				continue;
			if (edge->right == NULL || !nodes.contains(edge->right))
				continue;
			//examine only once
			if (edge->left != node)
				continue;
			//We have a comparison edge between two nodes in the subgraph
			//For now we don't support multiple encoding values with the same encoding....
			//So we enforce != constraints for every Set...
			if (edge->left != edge->right)
				generateEquals(edge->left, edge->right);
		}
		delete edgeit;
	}
	delete nodeit;
}

void EncodingSubGraph::computeComparisons() {
	SetIteratorEncodingNode *nodeit = nodes.iterator();
	while (nodeit->hasNext()) {
		EncodingNode *node = nodeit->next();
		SetIteratorEncodingEdge *edgeit = node->edges.iterator();
		while (edgeit->hasNext()) {
			EncodingEdge *edge = edgeit->next();
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

void EncodingSubGraph::orderEV(EncodingValue *earlier, EncodingValue *later) {
	earlier->larger.add(later);
}

void EncodingSubGraph::generateEquals(EncodingNode *left, EncodingNode *right) {
	Set *lset = left->s;
	Set *rset = right->s;
	uint lSize = lset->getSize(), rSize = rset->getSize();
	for (uint lindex = 0; lindex < lSize; lindex++) {
		for (uint rindex = 0; rindex < rSize; rindex++) {
			uint64_t lVal = lset->getElement(lindex);
			NodeValuePair nvp1(left, lVal);
			EncodingValue *lev = map.get(&nvp1);
			uint64_t rVal = rset->getElement(rindex);
			NodeValuePair nvp2(right, rVal);
			EncodingValue *rev = map.get(&nvp2);
			if (lev != rev) {
				if (lev->inComparison && rev->inComparison) {
					//Need to assign during comparison stage...
					//Thus promote to comparison
					if (lVal < rVal) {
						orderEV(lev, rev);
					} else {
						orderEV(rev, lev);
					}
				} else {
					lev->notequals.add(rev);
					rev->notequals.add(lev);
				}
			}
		}
	}
}

void EncodingSubGraph::generateComparison(EncodingNode *left, EncodingNode *right) {
	Set *lset = left->s;
	Set *rset = right->s;
	uint lindex = 0, rindex = 0;
	uint lSize = lset->getSize(), rSize = rset->getSize();
	uint64_t lVal = lset->getElement(lindex);
	NodeValuePair nvp1(left, lVal);
	EncodingValue *lev = map.get(&nvp1);
	lev->inComparison = true;
	uint64_t rVal = rset->getElement(rindex);
	NodeValuePair nvp2(right, rVal);
	EncodingValue *rev = map.get(&nvp2);
	rev->inComparison = true;
	EncodingValue *last = NULL;

	while (lindex < lSize || rindex < rSize) {
		if (last != NULL) {
			if (lev != NULL)
				orderEV(last, lev);
			if (rev != NULL && lev != rev)
				orderEV(last, rev);
		}
		if (lev != rev) {
			if (rev == NULL ||
					(lev != NULL && lVal < rVal)) {
				if (rev != NULL)
					orderEV(lev, rev);
				last = lev;
				if (++lindex < lSize) {
					lVal = lset->getElement(lindex);
					NodeValuePair nvpl(left, lVal);
					lev = map.get(&nvpl);
					lev->inComparison = true;
				} else
					lev = NULL;
			} else {
				if (lev != NULL)
					orderEV(rev, lev);
				last = rev;
				if (++rindex < rSize) {
					rVal = rset->getElement(rindex);
					NodeValuePair nvpr(right, rVal);
					rev = map.get(&nvpr);
					rev->inComparison = true;
				} else
					rev = NULL;
			}
		} else {
			last = lev;
			if (++lindex < lSize) {
				lVal = lset->getElement(lindex);
				NodeValuePair nvpl(left, lVal);
				lev = map.get(&nvpl);
				lev->inComparison = true;
			} else
				lev = NULL;

			if (++rindex < rSize) {
				rVal = rset->getElement(rindex);
				NodeValuePair nvpr(right, rVal);
				rev = map.get(&nvpr);
				rev->inComparison = true;
			} else
				rev = NULL;
		}
	}
}

void EncodingSubGraph::computeEncodingValue() {
	SetIteratorEncodingNode *nodeit = nodes.iterator();
	while (nodeit->hasNext()) {
		EncodingNode *node = nodeit->next();
		Set *set = node->s;
		uint setSize = set->getSize();
		for (uint i = 0; i < setSize; i++) {
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
	EncodingValue *ecv = new EncodingValue(value);
	values.add(ecv);
	HashsetEncodingNode discovered;
	Vector<EncodingNode *> tovisit;
	tovisit.push(node);
	discovered.add(node);
	while (tovisit.getSize() != 0) {
		EncodingNode *n = tovisit.last();tovisit.pop();
		//Add encoding node to structures
		ecv->nodes.add(n);
		NodeValuePair *nvp = new NodeValuePair(n, value);
		map.put(nvp, ecv);
		SetIteratorEncodingEdge *edgeit = n->edges.iterator();
		while (edgeit->hasNext()) {
			EncodingEdge *ee = edgeit->next();
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

