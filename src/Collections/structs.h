#ifndef STRUCTS_H
#define STRUCTS_H
#include "vector.h"
#include "hashtable.h"
#include "hashset.h"
#include "classlist.h"
#include "array.h"

ArrayDef(Element, Element *);
ArrayDef(Boolean, Boolean *);
ArrayDef(Set, Set *);

VectorDef(Table, Table *);
VectorDef(Set, Set *);
VectorDef(Boolean, Boolean *);
VectorDef(Constraint, Constraint *);
VectorDef(Function, Function *);
VectorDef(Predicate, Predicate *);
VectorDef(Element, Element *);
VectorDef(Order, Order *);
VectorDef(TableEntry, TableEntry *);
VectorDef(ASTNode, ASTNode *);
VectorDef(Int, uint64_t);

HashTableDef(Void, void *, void *);
HashTableDef(BoolConst, BooleanOrder *, Constraint *);

HashSetDef(Void, void *);

#endif
