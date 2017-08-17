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
VectorDef(BooleanOrder, BooleanOrder *);
VectorDef(Function, Function *);
VectorDef(Predicate, Predicate *);
VectorDef(Element, Element *);
VectorDef(Order, Order *);
VectorDef(TableEntry, TableEntry *);
VectorDef(ASTNode, ASTNode *);
VectorDef(Int, uint64_t);
VectorDef(OrderNode, OrderNode *);
VectorDef(OrderGraph, OrderGraph *);

HashTableDef(Void, void *, void *);
HashTableDef(OrderPair, OrderPair *, OrderPair *);

HashSetDef(Void, void *);
HashSetDef(TableEntry, TableEntry *);
HashSetDef(OrderNode, OrderNode *);
HashSetDef(OrderEdge, OrderEdge *);
#endif
