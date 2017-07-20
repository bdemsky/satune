#include "structs.h"
#include "mymemory.h"
#include "orderpair.h"
#include "tableentry.h"

VectorImpl(Table, Table *, 4);
VectorImpl(Set, Set *, 4);
VectorImpl(Boolean, Boolean *, 4);
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

static inline unsigned int order_pair_hash_Function(OrderPair* This){
	return (uint) (This->first << 2) ^ This->second;
}

static inline unsigned int order_pair_equals(OrderPair* key1, OrderPair* key2){
	return key1->first== key2->first && key1->second == key2->second;
}

static inline unsigned int table_entry_hash_Function(TableEntry* This){
	//http://isthe.com/chongo/tech/comp/fnv/
	unsigned int h = 2166136261;
	const unsigned int FNV_PRIME = 16777619;
	for(uint i=0; i<This->inputSize; i++){
		h ^= This->inputs[i];
		h *= FNV_PRIME;
	}
	return h;
}

static inline bool table_entry_equals(TableEntry* key1, TableEntry* key2){
	if(key1->inputSize != key2->inputSize)
		return false;
	for(uint i=0; i<key1->inputSize; i++)
		if(key1->inputs[i]!=key2->inputs[i])
			return false;
	return true;
}

HashTableImpl(OrderPair, OrderPair *, OrderPair *, order_pair_hash_Function, order_pair_equals, ourfree);
HashSetImpl(TableEntry, TableEntry*, table_entry_hash_Function, table_entry_equals);


