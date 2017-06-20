#ifndef PREDICATE_H
#define PREDICATE_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"

#define GETPREDICATETYPE(o) (((Predicate *)(o))->type)

struct Predicate {
	PredicateType type;
};

struct PredicateOperator {
	Predicate base;
	CompOp op;
	Set** domains;
	int numDomains;
};

struct PredicateTable {
	Predicate base;
	Set** domains;
	int numDomains;
	Table* table;
	UndefinedBehavior undefinedbehavior;
};


Predicate* allocPredicate(CompOp op, Set ** domain, uint numDomain);
void deletePredicate(Predicate* predicate);
#endif
