#include "encodinggraph.h"
#include "iterator.h"
#include "element.h"
#include "function.h"
#include "predicate.h"
#include "set.h"
#include "csolver.h"
#include "tunable.h"
#include "boolean.h"

EncodingGraph::EncodingGraph(CSolver * _solver) :
	solver(_solver) {
	

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
}

void EncodingGraph::mergeNodes(EncodingNode *first, EncodingNode *second) {
	EncodingSubGraph *graph1=graphMap.get(first);
	EncodingSubGraph *graph2=graphMap.get(second);
	if (graph1 == NULL && graph2 == NULL) {
		graph1 = new EncodingSubGraph();
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
		result->setEncoding((EdgeEncodingType)solver->getTuner()->getVarTunable(v1, v2, EDGEENCODING, &EdgeEncodingDesc));
		edgeMap.put(result, result);
	}
	return result;
}

EncodingNode::EncodingNode(Set *_s) :
	s(_s),
	numElements(0) {
}

uint EncodingNode::getSize() {
	return s->getSize();
}

VarType EncodingNode::getType() {
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
	if (discovered.add(e))
		n->numElements++;
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

EncodingSubGraph::EncodingSubGraph() {
}

void EncodingSubGraph::addNode(EncodingNode *n) {
	nodes.add(n);
	Set *s=n->s;
	uint size=s->getSize();
	for(uint i=0; i<size; i++) {
		uint64_t val=s->getElement(i);
		values.add(val);
	}
}

SetIteratorEncodingNode * EncodingSubGraph::nodeIterator() {
	return nodes.iterator();
}

uint EncodingSubGraph::computeIntersection(Set *s) {
	uint intersect=0;
	uint size=s->getSize();
	for(uint i=0; i<size; i++) {
		uint64_t val=s->getElement(i);
		if (values.contains(val))
			intersect++;
	}
	return intersect;
}

uint EncodingSubGraph::computeIntersection(EncodingSubGraph *g) {
	if (g->values.getSize() > values.getSize()) {
		//iterator over smaller set
		return g->computeIntersection(this);
	}
	
	uint intersect=0;
	SetIterator64Int * iter=g->values.iterator();
	while(iter->hasNext()) {
		uint64_t val=iter->next();
		if (values.contains(val))
			intersect++;
	}
	delete iter;
	return intersect;
}
