#include "function.h"
#include "table.h"
#include "set.h"


FunctionOperator::FunctionOperator(ArithOp _op, Set **domain, uint numDomain, Set *_range, OverFlowBehavior _overflowbehavior) : Function(OPERATORFUNC), op(_op), domains(domain, numDomain), range(_range), overflowbehavior(_overflowbehavior) {
}

FunctionTable::FunctionTable (Table *_table, UndefinedBehavior _undefBehavior) : Function(TABLEFUNC), table(_table), undefBehavior(_undefBehavior) {
}

uint64_t FunctionOperator::applyFunctionOperator(uint numVals, uint64_t *values) {
	ASSERT(numVals == 2);
	switch (op) {
	case ADD:
		return values[0] + values[1];
		break;
	case SUB:
		return values[0] - values[1];
		break;
	default:
		ASSERT(0);
	}
}

bool FunctionOperator::isInRangeFunction(uint64_t val) {
	return range->exists(val);
}

