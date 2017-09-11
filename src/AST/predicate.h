#ifndef PREDICATE_H
#define PREDICATE_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "astops.h"
#include "structs.h"
#include "common.h"

class Predicate {
public:
	Predicate(PredicateType _type) : type(_type) {}
	virtual ~Predicate() {}
	virtual Predicate *clone(CSolver *solver, CloneMap *map) {ASSERT(0); return NULL;}
	virtual void serialize(Serializer* serializer) = 0;
	PredicateType type;
	CMEMALLOC;
};

class PredicateOperator : public Predicate {
public:
	PredicateOperator(CompOp op, Set **domain, uint numDomain);
	bool evalPredicateOperator(uint64_t *inputs);
	Predicate *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer* serializer);
	Array<Set *> domains;
	CompOp getOp() {return op;}
	CMEMALLOC;
 private:
	CompOp op;
};

class PredicateTable : public Predicate {
public:
	PredicateTable(Table *table, UndefinedBehavior undefBehavior);
	Predicate *clone(CSolver *solver, CloneMap *map);
	virtual void serialize(Serializer* serializer);
	Table *table;
	UndefinedBehavior undefinedbehavior;
	CMEMALLOC;
};
#endif
