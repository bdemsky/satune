#include "function.h"
#include "table.h"
#include "set.h"
#include "csolver.h"

FunctionOperator::FunctionOperator(ArithOp _op, Set **domain, uint numDomain, Set *_range, OverFlowBehavior _overflowbehavior) : Function(OPERATORFUNC), op(_op), domains(domain, numDomain), range(_range), overflowbehavior(_overflowbehavior) {
}

FunctionTable::FunctionTable (Table *_table, UndefinedBehavior _undefBehavior) : Function(TABLEFUNC), table(_table), undefBehavior(_undefBehavior) {
}

uint64_t FunctionOperator::applyFunctionOperator(uint numVals, uint64_t *values) {
	ASSERT(numVals == 2);
	switch (op) {
	case SATC_ADD:
		return values[0] + values[1];
		break;
	case SATC_SUB:
		return values[0] - values[1];
		break;
	default:
		ASSERT(0);
	}
}

bool FunctionOperator::isInRangeFunction(uint64_t val) {
	return range->exists(val);
}

Function *FunctionOperator::clone(CSolver *solver, CloneMap *map) {
	Function *f = (Function *) map->get(this);
	if (f != NULL)
		return f;

	Set *array[domains.getSize()];
	for (uint i = 0; i < domains.getSize(); i++) {
		array[i] = domains.get(i)->clone(solver, map);
	}
	Set *rcopy = range->clone(solver, map);
	f = solver->createFunctionOperator(op, array, domains.getSize(), rcopy, overflowbehavior);
	map->put(this, f);
	return f;
}

Function *FunctionTable::clone(CSolver *solver, CloneMap *map) {
	Function *f = (Function *) map->get(this);
	if (f != NULL)
		return f;

	Table *tcopy = table->clone(solver, map);
	f = solver->completeTable(tcopy, undefBehavior);
	map->put(this, f);
	return f;
}
