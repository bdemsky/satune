#ifndef STRUCTS_H
#define STRUCTS_H
#include "cppvector.h"
#include "hashtable.h"
#include "hashset.h"
#include "classlist.h"
#include "array.h"
#include "corestructs.h"

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



typedef Hashset<TableEntry *, uintptr_t, 4, table_entry_hash_function, table_entry_equals> HashsetTableEntry;
typedef Hashset<OrderNode *, uintptr_t, 4, order_node_hash_function, order_node_equals> HashsetOrderNode;
typedef Hashset<OrderEdge *, uintptr_t, 4, order_edge_hash_function, order_edge_equals> HashsetOrderEdge;
typedef Hashset<OrderElement *, uintptr_t, 4, order_element_hash_function, order_element_equals> HashsetOrderElement;
typedef Hashtable<OrderNode *, HashsetOrderNode *, uintptr_t, 4> HashtableNodeToNodeSet;
typedef Hashtable<OrderPair *, OrderPair *, uintptr_t, 4, order_pair_hash_function, order_pair_equals> HashtableOrderPair;
typedef Hashtable<void *, void *, uintptr_t, 4> CloneMap;
typedef Hashtable<Order* , IntegerEncodingRecord*, uintptr_t, 4> HashtableOrderIntegerEncoding; 

typedef SetIterator<TableEntry *, uintptr_t, 4, table_entry_hash_function, table_entry_equals> SetIteratorTableEntry;
typedef SetIterator<OrderEdge *, uintptr_t, 4, order_edge_hash_function, order_edge_equals> SetIteratorOrderEdge;
typedef SetIterator<OrderNode *, uintptr_t, 4, order_node_hash_function, order_node_equals> SetIteratorOrderNode;


#endif
