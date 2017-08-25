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
	PredicateType type;
	MEMALLOC;
};

class PredicateOperator : public Predicate {
 public:
	PredicateOperator(CompOp op, Set **domain, uint numDomain);
	~PredicateOperator();
	bool evalPredicateOperator(uint64_t *inputs);
	CompOp op;
	ArraySet domains;
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
