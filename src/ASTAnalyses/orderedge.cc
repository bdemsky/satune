#include "orderedge.h"
#include "ordergraph.h"

OrderEdge::OrderEdge(OrderNode *_source, OrderNode *_sink) {
	source = _source;
	sink = _sink;
	polPos = false;
	polNeg = false;
	mustPos = false;
	mustNeg = false;
	pseudoPos = false;
}
