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
#include "boolean.h"

EncodingGraph::EncodingGraph(CSolver * _solver) :
	solver(_solver) {
}

int sortEncodingEdge(const void * p1, const void *p2) {
	const EncodingEdge * e1 = * (const EncodingEdge **) p1;
	const EncodingEdge * e2 = * (const EncodingEdge **) p2;
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
	while(it.hasNext()) {
		Element * e = it.next();
		switch(e->type) {
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

void EncodingGraph::encode() {
	SetIteratorEncodingSubGraph * itesg=subgraphs.iterator();
	while(itesg->hasNext()) {
		EncodingSubGraph *sg=itesg->next();
		sg->encode();
	}
	delete itesg;

	ElementIterator it(solver);
	while(it.hasNext()) {
		Element * e = it.next();
		switch(e->type) {
		case ELEMSET:
		case ELEMFUNCRETURN: {
			ElementEncoding *encoding=e->getElementEncoding();
			if (encoding->getElementEncodingType() == ELEM_UNASSIGNED) {
				//Do assignment...
			}
			break;
		}
		default:
			break;
		}
	}
	
}

void EncodingGraph::mergeNodes(EncodingNode *first, EncodingNode *second) {
	EncodingSubGraph *graph1=graphMap.get(first);
	EncodingSubGraph *graph2=graphMap.get(second);
	if (graph1 == NULL && graph2 == NULL) {
		graph1 = new EncodingSubGraph();
		subgraphs.add(graph1);
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
		SetIteratorEncodingNode * nodeit=graph2->nodeIterator();
		while(nodeit->hasNext()) {
			EncodingNode *node=nodeit->next();
			graph1->addNode(node);
			graphMap.put(node, graph1);
		}
		subgraphs.remove(graph2);
		delete nodeit;
		delete graph2;
	} else {
		ASSERT(graph1 != NULL && graph2 == NULL);
		graph1->addNode(second);
		graphMap.put(second, graph1);
	}
}

void EncodingGraph::processElement(Element *e) {
	uint size=e->parents.getSize();
	for(uint i=0;i<size;i++) {
		ASTNode * n = e->parents.get(i);
		switch(n->type) {
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
	Function *f=ef->getFunction();
	if (f->type==OPERATORFUNC) {
		FunctionOperator *fo=(FunctionOperator*)f;
		ASSERT(ef->inputs.getSize() == 2);
		EncodingNode *left=createNode(ef->inputs.get(0));
		EncodingNode *right=createNode(ef->inputs.get(1));
		if (left == NULL && right == NULL)
			return;
		EncodingNode *dst=createNode(ef);
		EncodingEdge *edge=getEdge(left, right, dst);
		edge->numArithOps++;
	}
}

void EncodingGraph::processPredicate(BooleanPredicate *b) {
	Predicate *p=b->getPredicate();
	if (p->type==OPERATORPRED) {
		PredicateOperator *po=(PredicateOperator *)p;
		ASSERT(b->inputs.getSize()==2);
		EncodingNode *left=createNode(b->inputs.get(0));
		EncodingNode *right=createNode(b->inputs.get(1));
		if (left == NULL || right == NULL)
			return;
		EncodingEdge *edge=getEdge(left, right, NULL);
		CompOp op=po->getOp();
		switch(op) {
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
	cost = 1.2 * cost; // fudge factor
	return NEXTPOW2(cost);
}

void EncodingGraph::decideEdges() {
	uint size=edgeVector.getSize();
	for(uint i=0; i<size; i++) {
		EncodingEdge *ee = edgeVector.get(i);
		EncodingNode *left = ee->left;
		EncodingNode *right = ee->right;
		
		if (ee->encoding != EDGE_UNASSIGNED ||
				left->encoding != BINARYINDEX ||
				right->encoding != BINARYINDEX)
			continue;
		
		uint64_t eeValue = ee->getValue();
		if (eeValue == 0)
			return;

		EncodingSubGraph *leftGraph = graphMap.get(left);
		EncodingSubGraph *rightGraph = graphMap.get(right);
		if (leftGraph == NULL && rightGraph !=NULL) {
			EncodingNode *tmp = left; left=right; right=tmp;
			EncodingSubGraph *tmpsg = leftGraph; leftGraph = rightGraph; rightGraph = tmpsg;
		}

		uint leftSize=0, rightSize=0, newSize=0;
		uint64_t totalCost=0;
		if (leftGraph == NULL && rightGraph == NULL) {
			leftSize=convertSize(left->getSize());
			rightSize=convertSize(right->getSize());
			newSize=convertSize(left->s->getUnionSize(right->s));
			newSize=(leftSize > newSize) ? leftSize: newSize;
			newSize=(rightSize > newSize) ? rightSize: newSize;
			totalCost = (newSize - leftSize) * left->elements.getSize() +
				(newSize - rightSize) * right->elements.getSize();
		} else if (leftGraph != NULL && rightGraph == NULL) {
			leftSize=convertSize(leftGraph->encodingSize);
			rightSize=convertSize(right->getSize());
			newSize=convertSize(leftGraph->estimateNewSize(right));
			newSize=(leftSize > newSize) ? leftSize: newSize;
			newSize=(rightSize > newSize) ? rightSize: newSize;
			totalCost = (newSize - leftSize) * leftGraph->numElements +
				(newSize - rightSize) * right->elements.getSize();
		} else {
			//Neither are null
			leftSize=convertSize(leftGraph->encodingSize);
			rightSize=convertSize(rightGraph->encodingSize);
			newSize=convertSize(leftGraph->estimateNewSize(rightGraph));
			newSize=(leftSize > newSize) ? leftSize: newSize;
			newSize=(rightSize > newSize) ? rightSize: newSize;
			totalCost = (newSize - leftSize) * leftGraph->numElements +
				(newSize - rightSize) * rightGraph->numElements;
		}
		double conversionfactor = 0.5;
		if ((totalCost * conversionfactor) < eeValue) {
			//add the edge
			mergeNodes(left, right);
		}
	}
}

static TunableDesc EdgeEncodingDesc(EDGE_UNASSIGNED, EDGE_MATCH, EDGE_UNASSIGNED);

EncodingEdge * EncodingGraph::getEdge(EncodingNode *left, EncodingNode *right, EncodingNode *dst) {
	EncodingEdge e(left, right, dst);
	EncodingEdge *result = edgeMap.get(&e);
	if (result == NULL) {
		result=new EncodingEdge(left, right, dst);
		VarType v1=left->getType();
		VarType v2=right->getType();
		if (v1 > v2) {
			VarType tmp=v2;
			v2=v1;
			v1=tmp;
		}

		if ((left != NULL && left->encoding==BINARYINDEX) &&
				(right != NULL) && right->encoding==BINARYINDEX) {
			EdgeEncodingType type=(EdgeEncodingType)solver->getTuner()->getVarTunable(v1, v2, EDGEENCODING, &EdgeEncodingDesc);
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

VarType EncodingNode::getType() const {
	return s->getType();
}

static TunableDesc NodeEncodingDesc(ELEM_UNASSIGNED, BINARYINDEX, ELEM_UNASSIGNED);

EncodingNode * EncodingGraph::createNode(Element *e) {
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
	uintptr_t hash=(((uintptr_t) edge->left) >> 2) ^ (((uintptr_t)edge->right) >> 4) ^ (((uintptr_t)edge->dst) >> 6);
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


