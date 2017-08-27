#include "predicate.h"
#include "boolean.h"
#include "set.h"
#include "table.h"

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
