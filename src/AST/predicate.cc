#include "predicate.h"
#include "boolean.h"
#include "set.h"
#include "table.h"
#include "csolver.h"

PredicateOperator::PredicateOperator(CompOp _op, Set **domain, uint numDomain) : Predicate(OPERATORPRED), op(_op), domains(domain, numDomain) {
}

PredicateTable::PredicateTable(Table *_table, UndefinedBehavior _undefBehavior) : Predicate(TABLEPRED), table(_table), undefinedbehavior(_undefBehavior) {
}

bool PredicateOperator::evalPredicateOperator(uint64_t *inputs) {
	switch (op) {
	case EQUALS:
		return inputs[0] == inputs[1];
	case LT:
		return inputs[0] < inputs[1];
	case GT:
		return inputs[0] > inputs[1];
	case LTE:
		return inputs[0] <= inputs[1];
	case GTE:
		return inputs[0] >= inputs[1];
	}
	ASSERT(0);
	return false;
}

Predicate *PredicateOperator::clone(CSolver *solver, CloneMap *map) {
	Predicate *p = (Predicate *) map->get(this);
	if (p != NULL)
		return p;

	Set *array[domains.getSize()];
	for (uint i = 0; i < domains.getSize(); i++)
		array[i] = domains.get(i)->clone(solver, map);

	p = solver->createPredicateOperator(op, array, domains.getSize());
	map->put(this, p);
	return p;
}

Predicate *PredicateTable::clone(CSolver *solver, CloneMap *map) {
	Predicate *p = (Predicate *) map->get(this);
	if (p != NULL)
		return p;

	p = solver->createPredicateTable(table->clone(solver, map), undefinedbehavior);
	map->put(this, p);
	return p;
}
