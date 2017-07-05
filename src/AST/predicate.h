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
	ArraySet domains;
};

struct PredicateTable {
	Predicate base;
	Table* table;
	UndefinedBehavior undefinedbehavior;
};


Predicate* allocPredicateOperator(CompOp op, Set ** domain, uint numDomain);
Predicate* allocPredicateTable(Table* table, UndefinedBehavior undefBehavior);
// size and result will be modified by this function!
void getEqualitySetIntersection(PredicateOperator* predicate, uint* size, uint64_t* result);
void deletePredicate(Predicate* predicate);
#endif
