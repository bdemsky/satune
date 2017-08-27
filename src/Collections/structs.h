#ifndef STRUCTS_H
#define STRUCTS_H
#include "cppvector.h"
#include "hashtable.h"
#include "hashset.h"
#include "classlist.h"
#include "array.h"

unsigned int table_entry_hash_function(TableEntry *This);
bool table_entry_equals(TableEntry *key1, TableEntry *key2);
unsigned int order_node_hash_function(OrderNode *This);
bool order_node_equals(OrderNode *key1, OrderNode *key2);
unsigned int order_edge_hash_function(OrderEdge *This);
bool order_edge_equals(OrderEdge *key1, OrderEdge *key2);
unsigned int order_element_hash_function(OrderElement *This);
bool order_element_equals(OrderElement *key1, OrderElement *key2);
unsigned int order_pair_hash_function(OrderPair *This);
bool order_pair_equals(OrderPair *key1, OrderPair *key2);

typedef HashSet<Boolean *, uintptr_t, 4> HashSetBoolean;
typedef HashSet<TableEntry *, uintptr_t, 4, table_entry_hash_function, table_entry_equals> HashSetTableEntry;
typedef HashSet<OrderNode *, uintptr_t, 4, order_node_hash_function, order_node_equals> HashSetOrderNode;
typedef HashSet<OrderEdge *, uintptr_t, 4, order_edge_hash_function, order_edge_equals> HashSetOrderEdge;
typedef HashSet<OrderElement *, uintptr_t, 4, order_element_hash_function, order_element_equals> HashSetOrderElement;
typedef HashTable<OrderNode *, HashSetOrderNode *, uintptr_t, 4> HashTableNodeToNodeSet;
typedef HashTable<OrderPair *, OrderPair *, uintptr_t, 4, order_pair_hash_function, order_pair_equals> HashTableOrderPair;
typedef HashTable<Order *, Order *, uintptr_t, 4> OrderMap;
typedef HashTable<Boolean *, Boolean *, uintptr_t, 4> BooleanMap;
typedef HashTable<Element *, Element *, uintptr_t, 4> ElementMap;
typedef HashTable<Set *, Set *, uintptr_t, 4> SetMap;

typedef struct CloneMap {
	OrderMap order;
	BooleanMap boolean;
	ElementMap element;
	SetMap set;
} CloneMap;

typedef HSIterator<TableEntry *, uintptr_t, 4, table_entry_hash_function, table_entry_equals> HSIteratorTableEntry;
typedef HSIterator<Boolean *, uintptr_t, 4> HSIteratorBoolean;
typedef HSIterator<OrderEdge *, uintptr_t, 4, order_edge_hash_function, order_edge_equals> HSIteratorOrderEdge;
typedef HSIterator<OrderNode *, uintptr_t, 4, order_node_hash_function, order_node_equals> HSIteratorOrderNode;


#endif
