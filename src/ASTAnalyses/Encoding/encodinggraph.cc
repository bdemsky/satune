#include "encodinggraph.h"
#include "iterator.h"
#include "element.h"

EncodingGraph::EncodingGraph(CSolver * _solver) :
	solver(_solver) {
	

}

EncodingNode * EncodingGraph::getNode(Element * element) {
	return NULL;
}

void EncodingGraph::buildGraph() {
	ElementIterator it(solver);
	while(it.hasNext()) {
		Element * e = it.next();
		processElement(e);
	}
}

void EncodingGraph::processElement(Element *e) {
	switch(e->type) {
	case ELEMSET: {
		break;
	}
	case ELEMFUNCRETURN: {
		break;
	}
	case ELEMCONST: {
		break;
	}
	default:
		ASSERT(0);
	}
}
