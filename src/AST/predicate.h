#ifndef PREDICATE_H
#define PREDICATE_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"


#define GETPREDICATETYPE(o) (((Predicate *)(o))->type)

struct Predicate {
	PredicateType type;
};

struct PredicateOperator {
	Predicate base;
	CompOp op;
	Set ** domains;
	uint numDomains;
};

struct PredicateTable {
	Predicate base;
	Table* table;
	UndefinedBehavior undefinedbehavior;
};


Predicate* allocPredicate(CompOp op, Set ** domain, uint numDomain);
void deletePredicate(Predicate* predicate);
#endif
