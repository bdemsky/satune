#ifndef PREDICATE_H
#define PREDICATE_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"

struct Predicate {
    enum CompOp op;
    VectorSet* domains;
};


Predicate* allocPredicate(enum CompOp op, Set ** domain, uint numDomain);
#endif
