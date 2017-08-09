#include "structs.h"
#include "mymemory.h"
#include "orderpair.h"
#include "tableentry.h"
#include "ordernode.h"
#include "orderedge.h"
#include "ordergraph.h"

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
VectorImpl(OrderNode, OrderNode*, 4);
VectorImpl(OrderGraph, OrderGraph*, 4);

inline unsigned int Ptr_hash_function(void * hash) {
	return (unsigned int)((int64)hash >> 4);
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

static inline unsigned int order_node_hash_Function(OrderNode* This){
	return (uint) ((int64)This->order << 2) ^ This->id;
	
}

static inline bool order_node_equals(OrderNode* key1, OrderNode* key2){
	return key1->id == key2->id && key1->order == key2->order;
}

static inline unsigned int order_edge_hash_Function(OrderEdge* This){
	return (uint) (( (int64)This->sink << 2)^((int64)This->source << 6) ) ^ (int64)This->order;
	
}

static inline bool order_edge_equals(OrderEdge* key1, OrderEdge* key2){
	return key1->sink == key2->sink && key1->source == key2->source && key1->order == key2->order;
}

static inline unsigned int node_info_hash_function(OrderNode * hash) {
	return (uint)((int64)hash >> 4);
}

static inline bool node_info_equals(OrderNode * key1, OrderNode * key2) {
	return key1 == key2;
}

HashTableImpl(OrderPair, OrderPair *, OrderPair *, order_pair_hash_Function, order_pair_equals, ourfree);
HashTableImpl(Node, OrderNode *, NodeInfo *, node_info_hash_function, node_info_equals, ourfree);

HashSetImpl(TableEntry, TableEntry*, table_entry_hash_Function, table_entry_equals);
HashSetImpl(OrderNode, OrderNode*, order_node_hash_Function, order_node_equals);
HashSetImpl(OrderEdge, OrderEdge*, order_edge_hash_Function, order_edge_equals);

