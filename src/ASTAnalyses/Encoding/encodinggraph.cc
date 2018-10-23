#include "encodinggraph.h"
#include "iterator.h"
#include "element.h"
#include "function.h"
#include "predicate.h"
#include "set.h"
#include "csolver.h"
#include "tunable.h"
#include "qsort.h"
#include "subgraph.h"
#include "elementencoding.h"

EncodingGraph::EncodingGraph(CSolver *_solver) :
	solver(_solver) {
}

EncodingGraph::~EncodingGraph() {
	subgraphs.resetAndDelete();
	encodingMap.resetAndDeleteVals();
	edgeMap.resetAndDeleteVals();
}

int sortEncodingEdge(const void *p1, const void *p2) {
	const EncodingEdge *e1 = *(const EncodingEdge **) p1;
	const EncodingEdge *e2 = *(const EncodingEdge **) p2;
	uint64_t v1 = e1->getValue();
	uint64_t v2 = e2->getValue();
	if (v1 < v2)
		return 1;
	else if (v1 == v2)
		return 0;
	else
		return -1;
}

void EncodingGraph::buildGraph() {
	ElementIterator it(solver);
	while (it.hasNext()) {
		Element *e = it.next();
		switch (e->type) {
		case ELEMSET:
		case ELEMFUNCRETURN:
			processElement(e);
			break;
		case ELEMCONST:
			break;
		default:
			ASSERT(0);
		}
	}
	bsdqsort(edgeVector.expose(), edgeVector.getSize(), sizeof(EncodingEdge *), sortEncodingEdge);
	decideEdges();
}


void EncodingGraph::validate() {
	SetIteratorBooleanEdge *it = solver->getConstraints();
	while (it->hasNext()) {
		BooleanEdge be = it->next();
		if (be->type == PREDICATEOP) {
			BooleanPredicate *b = (BooleanPredicate *)be.getBoolean();
			if (b->predicate->type == OPERATORPRED) {
				PredicateOperator *predicate = (PredicateOperator *) b->predicate;
				if (predicate->getOp() == SATC_EQUALS) {
					ASSERT(b->inputs.getSize() == 2);
					Element *e1 = b->inputs.get(0);
					Element *e2 = b->inputs.get(1);
					if (e1->type == ELEMCONST || e1->type == ELEMCONST)
						continue;
					ElementEncoding *enc1 = e1->getElementEncoding();
					ElementEncoding *enc2 = e2->getElementEncoding();
					ASSERT(enc1->getElementEncodingType() != ELEM_UNASSIGNED);
					ASSERT(enc2->getElementEncodingType() != ELEM_UNASSIGNED);
					if (enc1->getElementEncodingType() == enc2->getElementEncodingType() && enc1->getElementEncodingType() == BINARYINDEX && b->getFunctionEncoding()->type == CIRCUIT) {
						for (uint i = 0; i < enc1->encArraySize; i++) {
							if (enc1->isinUseElement(i)) {
								uint64_t val1 = enc1->encodingArray[i];
								if (enc2->isinUseElement(i)) {
									ASSERT(val1 == enc2->encodingArray[i]);
								} else {
									for (uint j = 0; j < enc2->encArraySize; j++) {
										if (enc2->isinUseElement(j)) {
											ASSERT(val1 != enc2->encodingArray[j]);
										}
									}
								}
							}
						}
					}
					//Now make sure that all the elements in the set are appeared in the encoding array!
					for (uint k = 0; k < b->inputs.getSize(); k++) {
						Element *e = b->inputs.get(k);
						ElementEncoding *enc = e->getElementEncoding();
						Set *s = e->getRange();
						for (uint i = 0; i < s->getSize(); i++) {
							uint64_t value = s->getElement(i);
							bool exist = false;
							for (uint j = 0; j < enc->encArraySize; j++) {
								if (enc->isinUseElement(j) && enc->encodingArray[j] == value) {
									exist = true;
									break;
								}
							}
							ASSERT(exist);
						}
					}
				}
			}
		}
	}
	delete it;
}


void EncodingGraph::encode() {
	if (solver->getTuner()->getTunable(ENCODINGGRAPHOPT, &offon) == 0)
		return;
	buildGraph();
	SetIteratorEncodingSubGraph *itesg = subgraphs.iterator();
	model_print("#SubGraph = %u\n", subgraphs.getSize());
	while (itesg->hasNext()) {
		EncodingSubGraph *sg = itesg->next();
		sg->encode();
	}
	delete itesg;

	ElementIterator it(solver);
	while (it.hasNext()) {
		Element *e = it.next();
		switch (e->type) {
		case ELEMSET:
		case ELEMFUNCRETURN: {
			ElementEncoding *encoding = e->getElementEncoding();
			if (encoding->getElementEncodingType() == ELEM_UNASSIGNED) {
				EncodingNode *n = getNode(e);
				if (n == NULL)
					continue;
				ElementEncodingType encodetype = n->getEncoding();
				encoding->setElementEncodingType(encodetype);
				if (encodetype == UNARY || encodetype == ONEHOT) {
					encoding->encodingArrayInitialization();
				} else if (encodetype == BINARYINDEX) {
					EncodingSubGraph *subgraph = graphMap.get(n);
					DEBUG("graphMap.get(subgraph=%p, n=%p)\n", subgraph, n);
					if (subgraph == NULL) {
						encoding->encodingArrayInitialization();
						continue;
					}
					uint encodingSize = subgraph->getEncodingMaxVal(n) + 1;
					uint paddedSize = encoding->getSizeEncodingArray(encodingSize);
					encoding->allocInUseArrayElement(paddedSize);
					encoding->allocEncodingArrayElement(paddedSize);
					Set *s = e->getRange();
					for (uint i = 0; i < s->getSize(); i++) {
						uint64_t value = s->getElement(i);
						uint encodingIndex = subgraph->getEncoding(n, value);
						encoding->setInUseElement(encodingIndex);
						ASSERT(encoding->isinUseElement(encodingIndex));
						encoding->encodingArray[encodingIndex] = value;
					}
				}
			}
			break;
		}
		default:
			break;
		}
		encodeParent(e);
	}
}

void EncodingGraph::encodeParent(Element *e) {
	uint size = e->parents.getSize();
	for (uint i = 0; i < size; i++) {
		ASTNode *n = e->parents.get(i);
		if (n->type == PREDICATEOP) {
			BooleanPredicate *b = (BooleanPredicate *)n;
			FunctionEncoding *fenc = b->getFunctionEncoding();
			if (fenc->getFunctionEncodingType() != FUNC_UNASSIGNED)
				continue;
			Predicate *p = b->getPredicate();
			if (p->type == OPERATORPRED) {
				PredicateOperator *po = (PredicateOperator *)p;
				ASSERT(b->inputs.getSize() == 2);
				EncodingNode *left = createNode(b->inputs.get(0));
				EncodingNode *right = createNode(b->inputs.get(1));
				if (left == NULL || right == NULL)
					return;
				EncodingEdge *edge = getEdge(left, right, NULL);
				if (edge != NULL) {
					EncodingSubGraph *leftGraph = graphMap.get(left);
					if (leftGraph != NULL && leftGraph == graphMap.get(right)) {
						fenc->setFunctionEncodingType(CIRCUIT);
					}
				}
			}
		}
	}
}

void EncodingGraph::mergeNodes(EncodingNode *first, EncodingNode *second) {
	EncodingSubGraph *graph1 = graphMap.get(first);
	DEBUG("graphMap.get(first=%p, graph1=%p)\n", first, graph1);
	EncodingSubGraph *graph2 = graphMap.get(second);
	DEBUG("graphMap.get(second=%p, graph2=%p)\n", second, graph2);
	if (graph1 == NULL)
		first->setEncoding(BINARYINDEX);
	if (graph2 == NULL)
		second->setEncoding(BINARYINDEX);

	if (graph1 == NULL && graph2 == NULL) {
		graph1 = new EncodingSubGraph();
		subgraphs.add(graph1);
		DEBUG("graphMap.put(first=%p, graph1=%p)\n", first, graph1);
		graphMap.put(first, graph1);
		graph1->addNode(first);
	}
	if (graph1 == NULL && graph2 != NULL) {
		graph1 = graph2;
		graph2 = NULL;
		EncodingNode *tmp = second;
		second = first;
		first = tmp;
	}
	if (graph1 != NULL && graph2 != NULL) {
		if (graph1 == graph2)
			return;

		SetIteratorEncodingNode *nodeit = graph2->nodeIterator();
		while (nodeit->hasNext()) {
			EncodingNode *node = nodeit->next();
			graph1->addNode(node);
			DEBUG("graphMap.put(node=%p, graph1=%p)\n", node, graph1);
			graphMap.put(node, graph1);
		}
		subgraphs.remove(graph2);
		delete nodeit;
		DEBUG("Deleting graph2 =%p \n", graph2);
		delete graph2;
	} else {
		ASSERT(graph1 != NULL && graph2 == NULL);
		graph1->addNode(second);
		DEBUG("graphMap.put(first=%p, graph1=%p)\n", first, graph1);
		graphMap.put(second, graph1);
	}
}

void EncodingGraph::processElement(Element *e) {
	uint size = e->parents.getSize();
	for (uint i = 0; i < size; i++) {
		ASTNode *n = e->parents.get(i);
		switch (n->type) {
		case PREDICATEOP:
			processPredicate((BooleanPredicate *)n);
			break;
		case ELEMFUNCRETURN:
			processFunction((ElementFunction *)n);
			break;
		default:
			ASSERT(0);
		}
	}
}

void EncodingGraph::processFunction(ElementFunction *ef) {
	Function *f = ef->getFunction();
	if (f->type == OPERATORFUNC) {
		FunctionOperator *fo = (FunctionOperator *)f;
		ASSERT(ef->inputs.getSize() == 2);
		EncodingNode *left = createNode(ef->inputs.get(0));
		EncodingNode *right = createNode(ef->inputs.get(1));
		if (left == NULL && right == NULL)
			return;
		EncodingNode *dst = createNode(ef);
		EncodingEdge *edge = createEdge(left, right, dst);
		edge->numArithOps++;
	}
}

void EncodingGraph::processPredicate(BooleanPredicate *b) {
	Predicate *p = b->getPredicate();
	if (p->type == OPERATORPRED) {
		PredicateOperator *po = (PredicateOperator *)p;
		ASSERT(b->inputs.getSize() == 2);
		EncodingNode *left = createNode(b->inputs.get(0));
		EncodingNode *right = createNode(b->inputs.get(1));
		if (left == NULL || right == NULL)
			return;
		EncodingEdge *edge = createEdge(left, right, NULL);
		CompOp op = po->getOp();
		switch (op) {
		case SATC_EQUALS:
			edge->numEquals++;
			break;
		case SATC_LT:
		case SATC_LTE:
		case SATC_GT:
		case SATC_GTE:
			edge->numComparisons++;
			break;
		default:
			ASSERT(0);
		}
	}
}

uint convertSize(uint cost) {
	return NEXTPOW2(cost);
}

void EncodingGraph::decideEdges() {
	uint size = edgeVector.getSize();
	for (uint i = 0; i < size; i++) {
		EncodingEdge *ee = edgeVector.get(i);
		EncodingNode *left = ee->left;
		EncodingNode *right = ee->right;

		if (ee->encoding != EDGE_UNASSIGNED ||
				!left->couldBeBinaryIndex() ||
				!right->couldBeBinaryIndex())
			continue;

		uint64_t eeValue = ee->getValue();
		if (eeValue == 0)
			return;

		EncodingSubGraph *leftGraph = graphMap.get(left);
		DEBUG("graphMap.get(left=%p, leftgraph=%p)\n", left, leftGraph);
		EncodingSubGraph *rightGraph = graphMap.get(right);
		DEBUG("graphMap.get(right=%p, rightgraph=%p)\n", right, rightGraph);
		if (leftGraph == NULL && rightGraph != NULL) {
			EncodingNode *tmp = left; left = right; right = tmp;
			EncodingSubGraph *tmpsg = leftGraph; leftGraph = rightGraph; rightGraph = tmpsg;
		}

		uint leftSize = 0, rightSize = 0, newSize = 0, min = 0;
		bool merge = false;
		if (leftGraph == NULL && rightGraph == NULL) {
			leftSize = convertSize(left->getSize());
			rightSize = convertSize(right->getSize());
			newSize = convertSize(left->s->getUnionSize(right->s));
			newSize = (leftSize > newSize) ? leftSize : newSize;
			newSize = (rightSize > newSize) ? rightSize : newSize;
			min = rightSize > leftSize ? leftSize : rightSize;
			merge = left->measureSimilarity(right) > 1.5 || min == newSize;
		} else if (leftGraph != NULL && rightGraph == NULL) {
			leftSize = convertSize(leftGraph->numValues());
			rightSize = convertSize(right->getSize());
			newSize = convertSize(leftGraph->estimateNewSize(right));
			newSize = (leftSize > newSize) ? leftSize : newSize;
			newSize = (rightSize > newSize) ? rightSize : newSize;
			min = rightSize > leftSize ? leftSize : rightSize;
//			model_print("Merge=%s\tsimilarity=%f\n", max==newSize?"TRUE":"FALSE", left->measureSimilarity(right));
			merge = leftGraph->measureSimilarity(right) > 1.5 || min == newSize;
		} else {
			//Neither are null
			leftSize = convertSize(leftGraph->numValues());
			rightSize = convertSize(rightGraph->numValues());
			newSize = convertSize(leftGraph->estimateNewSize(rightGraph));
//			model_print("MergingSubGraphs: left=%u\tright=%u\tnewSize=%u\n", leftSize, rightSize, newSize);
			newSize = (leftSize > newSize) ? leftSize : newSize;
			newSize = (rightSize > newSize) ? rightSize : newSize;
			min = rightSize > leftSize ? leftSize : rightSize;
//			model_print("Merge=%s\tsimilarity=%f\n", max==newSize?"TRUE":"FALSE", leftGraph->measureSimilarity(right));
			merge = leftGraph->measureSimilarity(rightGraph) > 1.5 || min == newSize;
		}
		if (merge) {
			//add the edge
			mergeNodes(left, right);
		}
	}
}

static TunableDesc EdgeEncodingDesc(EDGE_UNASSIGNED, EDGE_MATCH, EDGE_UNASSIGNED);

EncodingEdge *EncodingGraph::getEdge(EncodingNode *left, EncodingNode *right, EncodingNode *dst) {
	EncodingEdge e(left, right, dst);
	EncodingEdge *result = edgeMap.get(&e);
	return result;
}

EncodingEdge *EncodingGraph::createEdge(EncodingNode *left, EncodingNode *right, EncodingNode *dst) {
	EncodingEdge e(left, right, dst);
	EncodingEdge *result = edgeMap.get(&e);
	if (result == NULL) {
		result = new EncodingEdge(left, right, dst);
		VarType v1 = left->getType();
		VarType v2 = right->getType();
		if (v1 > v2) {
			VarType tmp = v2;
			v2 = v1;
			v1 = tmp;
		}

		if ((left != NULL && left->couldBeBinaryIndex()) &&
				(right != NULL) && right->couldBeBinaryIndex()) {
			EdgeEncodingType type = (EdgeEncodingType)solver->getTuner()->getVarTunable(v1, v2, EDGEENCODING, &EdgeEncodingDesc);
			result->setEncoding(type);
			if (type == EDGE_MATCH) {
				mergeNodes(left, right);
			}
		}
		edgeMap.put(result, result);
		edgeVector.push(result);
		if (left != NULL)
			left->edges.add(result);
		if (right != NULL)
			right->edges.add(result);
		if (dst != NULL)
			dst->edges.add(result);
	}
	return result;
}

EncodingNode::EncodingNode(Set *_s) :
	s(_s) {
}

uint EncodingNode::getSize() const {
	return s->getSize();
}

uint64_t EncodingNode::getIndex(uint index) {
	return s->getElement(index);
}

VarType EncodingNode::getType() const {
	return s->getType();
}

double EncodingNode::measureSimilarity(EncodingNode *node) {
	uint common = 0;
	for (uint i = 0, j = 0; i < s->getSize() && j < node->s->getSize(); ) {
		uint64_t item = s->getElement(i);
		uint64_t item2 = node->s->getElement(j);
		if (item < item2)
			i++;
		else if (item2 > item)
			j++;
		else {
			i++;
			j++;
			common++;
		}
	}

	return common * 1.0 / s->getSize() + common * 1.0 / node->getSize();
}

EncodingNode *EncodingGraph::createNode(Element *e) {
	if (e->type == ELEMCONST)
		return NULL;
	Set *s = e->getRange();
	EncodingNode *n = encodingMap.get(s);
	if (n == NULL) {
		n = new EncodingNode(s);
		n->setEncoding((ElementEncodingType)solver->getTuner()->getVarTunable(n->getType(), NODEENCODING, &NodeEncodingDesc));

		encodingMap.put(s, n);
	}
	n->addElement(e);
	return n;
}

EncodingNode *EncodingGraph::getNode(Element *e) {
	if (e->type == ELEMCONST)
		return NULL;
	Set *s = e->getRange();
	EncodingNode *n = encodingMap.get(s);
	return n;
}

void EncodingNode::addElement(Element *e) {
	elements.add(e);
}

EncodingEdge::EncodingEdge(EncodingNode *_l, EncodingNode *_r) :
	left(_l),
	right(_r),
	dst(NULL),
	encoding(EDGE_UNASSIGNED),
	numArithOps(0),
	numEquals(0),
	numComparisons(0)
{
}

EncodingEdge::EncodingEdge(EncodingNode *_left, EncodingNode *_right, EncodingNode *_dst) :
	left(_left),
	right(_right),
	dst(_dst),
	encoding(EDGE_UNASSIGNED),
	numArithOps(0),
	numEquals(0),
	numComparisons(0)
{
}

uint hashEncodingEdge(EncodingEdge *edge) {
	uintptr_t hash = (((uintptr_t) edge->left) >> 2) ^ (((uintptr_t)edge->right) >> 4) ^ (((uintptr_t)edge->dst) >> 6);
	return (uint) hash;
}

bool equalsEncodingEdge(EncodingEdge *e1, EncodingEdge *e2) {
	return e1->left == e2->left && e1->right == e2->right && e1->dst == e2->dst;
}

uint64_t EncodingEdge::getValue() const {
	uint lSize = (left != NULL) ? left->getSize() : 1;
	uint rSize = (right != NULL) ? right->getSize() : 1;
	uint min = (lSize < rSize) ? lSize : rSize;
	return numEquals * min + numComparisons * lSize * rSize;
}


