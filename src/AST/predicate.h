#ifndef PREDICATE_H
#define PREDICATE_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"

#define GETPREDICATETYPE(o) (((Predicate *)(o))->type)

class Predicate {
public:
	Predicate(PredicateType _type) : type(_type) {}
	virtual ~Predicate() {}
	PredicateType type;
	MEMALLOC;
};

class PredicateOperator : public Predicate {
public:
	PredicateOperator(CompOp op, Set **domain, uint numDomain);
	bool evalPredicateOperator(uint64_t *inputs);
	CompOp op;
	Array<Set *> domains;
	MEMALLOC;
};

class PredicateTable : public Predicate {
public:
	PredicateTable(Table *table, UndefinedBehavior undefBehavior);
	Table *table;
	UndefinedBehavior undefinedbehavior;
	MEMALLOC;
};
#endif
