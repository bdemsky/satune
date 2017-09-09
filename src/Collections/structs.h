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


typedef Hashset<TableEntry *, uintptr_t, PTRSHIFT, table_entry_hash_function, table_entry_equals> HashsetTableEntry;
typedef Hashset<OrderNode *, uintptr_t, PTRSHIFT, order_node_hash_function, order_node_equals> HashsetOrderNode;
typedef Hashset<OrderEdge *, uintptr_t, PTRSHIFT, order_edge_hash_function, order_edge_equals> HashsetOrderEdge;
typedef Hashset<OrderElement *, uintptr_t, PTRSHIFT, order_element_hash_function, order_element_equals> HashsetOrderElement;
typedef Hashset<Boolean *, uintptr_t, PTRSHIFT> HashsetBoolean;
typedef Hashset<Element *, uintptr_t, PTRSHIFT> HashsetElement;
typedef SetIterator<Boolean *, uintptr_t, PTRSHIFT> SetIteratorBoolean;

typedef Hashtable<OrderNode *, HashsetOrderNode *, uintptr_t, PTRSHIFT> HashtableNodeToNodeSet;
typedef Hashtable<OrderPair *, OrderPair *, uintptr_t, PTRSHIFT, order_pair_hash_function, order_pair_equals> HashtableOrderPair;
typedef Hashtable<void *, void *, uintptr_t, PTRSHIFT> CloneMap;


typedef Hashtable<Set *, EncodingNode *, uintptr_t, PTRSHIFT> HashtableEncoding;

typedef SetIterator<TableEntry *, uintptr_t, PTRSHIFT, table_entry_hash_function, table_entry_equals> SetIteratorTableEntry;
typedef SetIterator<OrderEdge *, uintptr_t, PTRSHIFT, order_edge_hash_function, order_edge_equals> SetIteratorOrderEdge;
typedef SetIterator<OrderNode *, uintptr_t, PTRSHIFT, order_node_hash_function, order_node_equals> SetIteratorOrderNode;
typedef SetIterator<OrderElement *, uintptr_t, PTRSHIFT, order_element_hash_function, order_element_equals> SetIteratorOrderElement;

#endif
