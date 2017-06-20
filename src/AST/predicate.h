#ifndef PREDICATE_H
#define PREDICATE_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"

struct Predicate {
	CompOp op;
	VectorSet* domains;
};


Predicate* allocPredicate(CompOp op, Set ** domain, uint numDomain);
void deletePredicate(Predicate* predicate);
#endif
