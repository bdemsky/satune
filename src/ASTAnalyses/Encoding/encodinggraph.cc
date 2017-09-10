#include "encodinggraph.h"
#include "iterator.h"
#include "element.h"
#include "function.h"
#include "predicate.h"
#include "set.h"

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

EncodingEdge * EncodingGraph::getEdge(EncodingNode *left, EncodingNode *right, EncodingNode *dst) {
	EncodingEdge e(left, right, dst);
	EncodingEdge *result = edgeMap.get(&e);
	if (result == NULL) {
		result=new EncodingEdge(left, right, dst);
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

EncodingNode * EncodingGraph::createNode(Element *e) {
	if (e->type == ELEMCONST)
		return NULL;
	Set *s = e->getRange();
	EncodingNode *n = encodingMap.get(s);
	if (n == NULL) {
		n = new EncodingNode(s);
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
	numArithOps(0),
	numEquals(0),
	numComparisons(0)
{
}

EncodingEdge::EncodingEdge(EncodingNode *_left, EncodingNode *_right, EncodingNode *_dst) :
	left(_left),
	right(_right),
	dst(_dst),
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
