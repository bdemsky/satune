#include "mymemory.h"
#include "structs.h"
#include "orderpair.h"
#include "tableentry.h"
#include "ordernode.h"
#include "orderedge.h"
#include "ordergraph.h"
#include "orderelement.h"

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

static inline unsigned int order_element_hash_function(OrderElement* This) {
	return (uint)This->item;
}

static inline bool order_element_equals(OrderElement* key1, OrderElement* key2) {
	return key1->item == key2->item;
}


HashSetImpl(Boolean, Boolean *, Ptr_hash_function, Ptr_equals);
HashSetImpl(TableEntry, TableEntry *, table_entry_hash_function, table_entry_equals);
HashSetImpl(OrderNode, OrderNode *, order_node_hash_function, order_node_equals);
HashSetImpl(OrderEdge, OrderEdge *, order_edge_hash_function, order_edge_equals);
HashSetImpl(OrderElement, OrderElement *, order_element_hash_function, order_element_equals);

HashTableImpl(NodeToNodeSet, OrderNode *, HashSetOrderNode *, Ptr_hash_function, Ptr_equals, deleteHashSetOrderNode);
HashTableImpl(OrderPair, OrderPair *, OrderPair *, order_pair_hash_function, order_pair_equals, ourfree);
