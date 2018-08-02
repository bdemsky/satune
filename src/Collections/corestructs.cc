#include "classlist.h"
#include "corestructs.h"
#include "boolean.h"

void BooleanEdge::print(){
	if (isNegated())
		model_print("!");
	getBoolean()->print();
	model_print("\n");
}
