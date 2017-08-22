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
VectorImpl(BooleanOrder, BooleanOrder *, 4);
VectorImpl(Function, Function *, 4);
VectorImpl(Predicate, Predicate *, 4);
VectorImpl(Element, Element *, 4);
VectorImpl(Order, Order *, 4);
VectorImpl(TableEntry, TableEntry *, 4);
VectorImpl(ASTNode, ASTNode *, 4);
VectorImpl(Int, uint64_t, 4);
VectorImpl(OrderNode, OrderNode *, 4);
VectorImpl(OrderGraph, OrderGraph *, 4);

static inline unsigned int Ptr_hash_function(void *hash) {
	return (unsigned int)((int64)hash >> 4);
}

static inline bool Ptr_equals(void *key1, void *key2) {
	return key1 == key2;
}

static inline unsigned int order_pair_hash_function(OrderPair *This) {
	return (uint) (This->first << 2) ^ This->second;
}

static inline unsigned int order_pair_equals(OrderPair *key1, OrderPair *key2) {
	return key1->first == key2->first && key1->second == key2->second;
}

static inline unsigned int table_entry_hash_function(TableEntry *This) {
	unsigned int h = 0;
	for (uint i = 0; i < This->inputSize; i++) {
		h += This->inputs[i];
		h *= 31;
	}
	return h;
}

static inline bool table_entry_equals(TableEntry *key1, TableEntry *key2) {
	if (key1->inputSize != key2->inputSize)
		return false;
	for (uint i = 0; i < key1->inputSize; i++)
		if (key1->inputs[i] != key2->inputs[i])
			return false;
	return true;
}

static inline unsigned int order_node_hash_function(OrderNode *This) {
	return (uint) This->id;

}

static inline bool order_node_equals(OrderNode *key1, OrderNode *key2) {
	return key1->id == key2->id;
}

static inline unsigned int order_edge_hash_function(OrderEdge *This) {
	return (uint) (((uintptr_t)This->sink) ^ ((uintptr_t)This->source << 4));
}

static inline bool order_edge_equals(OrderEdge *key1, OrderEdge *key2) {
	return key1->sink == key2->sink && key1->source == key2->source;
}

HashTableImpl(OrderPair, OrderPair *, OrderPair *, order_pair_hash_function, order_pair_equals, ourfree);

HashSetImpl(Boolean, Boolean *, Ptr_hash_function, Ptr_equals);
HashSetImpl(TableEntry, TableEntry *, table_entry_hash_function, table_entry_equals);
HashSetImpl(OrderNode, OrderNode *, order_node_hash_function, order_node_equals);
HashSetImpl(OrderEdge, OrderEdge *, order_edge_hash_function, order_edge_equals);

HashTableImpl(NodeToNodeSet, OrderNode *, HashSetOrderNode *, Ptr_hash_function, Ptr_equals, deleteHashSetOrderNode);
