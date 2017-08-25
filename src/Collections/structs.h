#ifndef STRUCTS_H
#define STRUCTS_H
#include "cppvector.h"
#include "hashtable.h"
#include "hashset.h"
#include "classlist.h"
#include "array.h"

HashTableDef(Void, void *, void *);
HashTableDef(OrderPair, OrderPair *, OrderPair *);

HashSetDef(Boolean, Boolean *);
HashSetDef(TableEntry, TableEntry *);
HashSetDef(OrderNode, OrderNode *);
HashSetDef(OrderEdge, OrderEdge *);
HashSetDef(OrderElement, OrderElement *);

HashTableDef(NodeToNodeSet, OrderNode *, HashSetOrderNode *);
#endif
