#include "orderpair.h"


OrderPair::OrderPair(uint64_t _first, uint64_t _second, Edge _constraint) :
	first(_first),
	second(_second),
	constraint(_constraint) {
}

OrderPair::OrderPair() :
	first(0),
	second(0),
	constraint(E_NULL) {
}
