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


VectorDef(Table, Table *, 4);
VectorDef(Set, Set *, 4);
VectorDef(Boolean, Boolean *, 4);
VectorDef(Constraint, Constraint *, 4);
VectorDef(Function, Function *, 4);
VectorDef(Predicate, Predicate *, 4);
VectorDef(Element, Element *, 4);
VectorDef(Order, Order *, 4);
VectorDef(TableEntry, TableEntry *, 4);
VectorDef(ASTNode, ASTNode *, 4);
VectorDef(FunctionEncoding, FunctionEncoding *, 4);
VectorDef(ElementEncoding, ElementEncoding *, 4);
VectorDef(Int, uint64_t, 4);


inline unsigned int Ptr_hash_function(void * hash) {
	return (unsigned int)((uint64_t)hash >> 4);
}

inline bool Ptr_equals(void * key1, void * key2) {
	return key1 == key2;
}

HashTableDef(Void, void *, void *, Ptr_hash_function, Ptr_equals);
HashSetDef(Void, void *, Ptr_hash_function, Ptr_equals);


#endif
