#include "classlist.h"
#include "corestructs.h"
#include "boolean.h"

void BooleanEdge::print(){
	if (isNegated())
		model_print("!");
	b->print();
	model_print("\n");
}