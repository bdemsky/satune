#include "structs.h"
#include "mymemory.h"
#include "order.h"

VectorImpl(Table, Table *, 4);
VectorImpl(Set, Set *, 4);
VectorImpl(Boolean, Boolean *, 4);
VectorImpl(Constraint, Constraint *, 4);
VectorImpl(Function, Function *, 4);
VectorImpl(Predicate, Predicate *, 4);
VectorImpl(Element, Element *, 4);
VectorImpl(Order, Order *, 4);
VectorImpl(TableEntry, TableEntry *, 4);
VectorImpl(ASTNode, ASTNode *, 4);
VectorImpl(Int, uint64_t, 4);

inline unsigned int Ptr_hash_function(void * hash) {
	return (unsigned int)((uint64_t)hash >> 4);
}

inline bool Ptr_equals(void * key1, void * key2) {
	return key1 == key2;
}

inline unsigned int BooleanOrder_hash_Function(BooleanOrder* This){
	return This->first ^ This->second;
}

inline unsigned int BooleanOrder_equals(BooleanOrder* key1, BooleanOrder* key2){
	return key1->first== key2->first && key1->second == key2->second;
}

HashTableImpl(BoolConst, BooleanOrder *, Constraint *, BooleanOrder_hash_Function, BooleanOrder_equals);
