#include "predicate.h"
#include "boolean.h"
#include "set.h"
#include "table.h"
#include "csolver.h"

PredicateOperator::PredicateOperator(CompOp _op) : Predicate(OPERATORPRED), op(_op) {
}

PredicateTable::PredicateTable(Table *_table, UndefinedBehavior _undefBehavior) : Predicate(TABLEPRED), table(_table), undefinedbehavior(_undefBehavior) {
}

bool PredicateOperator::evalPredicateOperator(uint64_t *inputs) {
	switch (op) {
	case SATC_EQUALS:
		return inputs[0] == inputs[1];
	case SATC_LT:
		return inputs[0] < inputs[1];
	case SATC_GT:
		return inputs[0] > inputs[1];
	case SATC_LTE:
		return inputs[0] <= inputs[1];
	case SATC_GTE:
		return inputs[0] >= inputs[1];
	}
	ASSERT(0);
	return false;
}

Predicate *PredicateOperator::clone(CSolver *solver, CloneMap *map) {
	Predicate *p = (Predicate *) map->get(this);
	if (p != NULL)
		return p;

	p = solver->createPredicateOperator(op);
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

void PredicateTable::serialize(Serializer *serializer) {
	if (serializer->isSerialized(this))
		return;
	serializer->addObject(this);

	table->serialize(serializer);

	ASTNodeType type = PREDTABLETYPE;
	serializer->mywrite(&type, sizeof(ASTNodeType));
	PredicateTable *This = this;
	serializer->mywrite(&This, sizeof(PredicateTable *));
	serializer->mywrite(&table, sizeof(Table *));
	serializer->mywrite(&undefinedbehavior, sizeof(UndefinedBehavior));
}

void PredicateTable::print() {
	model_print("{PredicateTable:\n");
	table->print();
	model_print("}\n");
}

void PredicateOperator::serialize(Serializer *serializer) {
	if (serializer->isSerialized(this))
		return;
	serializer->addObject(this);

	ASTNodeType type = PREDOPERTYPE;
	serializer->mywrite(&type, sizeof(ASTNodeType));
	PredicateOperator *This = this;
	serializer->mywrite(&This, sizeof(PredicateOperator *));
	serializer->mywrite(&op, sizeof(CompOp));
}

void PredicateOperator::print() {
	const char *names[] = {"==", "<", ">", "<=", ">="};

	model_print("PredicateOperator: %s\n", names[(int)op]);
}



