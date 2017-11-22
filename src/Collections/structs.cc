#include "mymemory.h"
#include "orderpair.h"
#include "tableentry.h"
#include "ordernode.h"
#include "orderedge.h"
#include "ordergraph.h"
#include "orderelement.h"
#include "structs.h"
#include "decomposeorderresolver.h"

#define HASHNEXT(hash, newval) {hash+=newval; hash += hash << 10; hash ^= hash >>6;}
#define HASHFINAL(hash) {hash += hash <<3; hash ^= hash >> 11; hash += hash << 15;}

unsigned int table_entry_hash_function(TableEntry *This) {
	unsigned int h = 0;
	for (uint i = 0; i < This->inputSize; i++) {
		HASHNEXT(h, This->inputs[i]);
	}
	HASHFINAL(h);
	return h;
}

bool table_entry_equals(TableEntry *key1, TableEntry *key2) {
	if (key1->inputSize != key2->inputSize)
		return false;
	for (uint i = 0; i < key1->inputSize; i++)
		if (key1->inputs[i] != key2->inputs[i])
			return false;
	return true;
}

unsigned int order_node_hash_function(OrderNodeKey *This) {
	return (uint) This->id;
}

bool order_node_equals(OrderNodeKey *key1, OrderNodeKey *key2) {
	return key1->id == key2->id;
}

unsigned int order_edge_hash_function(OrderEdge *This) {
	uint hash=0;
	HASHNEXT(hash, (uint)(uintptr_t) This->sink);
	HASHNEXT(hash, (uint)(uintptr_t) This->source);
	HASHFINAL(hash);
	return hash;
}

bool order_edge_equals(OrderEdge *key1, OrderEdge *key2) {
	return key1->sink == key2->sink && key1->source == key2->source;
}

unsigned int order_element_hash_function(OrderElement *This) {
	return This->getHash();
}

bool order_element_equals(OrderElement *key1, OrderElement *key2) {
	return key1->equals(key2);
}

unsigned int order_pair_hash_function(OrderPair *This) {
	uint hash=0;
	HASHNEXT(hash, This->first);
	HASHNEXT(hash, This->second);
	HASHFINAL(hash);
	return hash;
}

bool order_pair_equals(OrderPair *key1, OrderPair *key2) {
	return key1->first == key2->first && key1->second == key2->second;
}

unsigned int doredge_hash_function(DOREdge *key) {
	uint hash=0;
	HASHNEXT(hash, (uint) key->newfirst);
	HASHNEXT(hash, (uint) key->newsecond);
	HASHFINAL(hash);
	return hash;
}

bool doredge_equals(DOREdge *key1, DOREdge *key2) {
	return key1->newfirst == key2->newfirst &&
				 key1->newsecond == key2->newsecond;
}
