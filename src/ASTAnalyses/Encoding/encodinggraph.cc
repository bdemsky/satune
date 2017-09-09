#include "encodinggraph.h"
#include "iterator.h"
#include "element.h"
#include "function.h"

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
		ArithOp op=fo->op;
	}
}

void EncodingGraph::processPredicate(BooleanPredicate *b) {

}

EncodingNode * EncodingGraph::createNode(Element *e) {
	Set *s = e->getRange();
	EncodingNode *n = encodingMap.get(s);
	if (n == NULL) {
		n = new EncodingNode(s);
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
	dst(NULL)
{
}

EncodingEdge::EncodingEdge(EncodingNode *_left, EncodingNode *_right, EncodingNode *_dst) :
	left(_left),
	right(_right),
	dst(_dst)
{
}

uint hashEncodingEdge(EncodingEdge *edge) {
	uintptr_t hash=(((uintptr_t) edge->left) >> 2) ^ (((uintptr_t)edge->right) >> 4) ^ (((uintptr_t)edge->dst) >> 6);
	return (uint) hash;
}

bool equalsEncodingEdge(EncodingEdge *e1, EncodingEdge *e2) {
	return e1->left == e2->left && e1->right == e2->right && e1->dst == e2->dst;
}
