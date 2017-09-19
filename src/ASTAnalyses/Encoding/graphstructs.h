#ifndef GRAPHSTRUCTS_H
#define GRAPHSTRUCTS_H
#include "classlist.h"
#include "structs.h"

uint hashEncodingEdge(EncodingEdge *edge);
bool equalsEncodingEdge(EncodingEdge *e1, EncodingEdge *e2);
class EncodingSubGraph;


typedef Hashtable<EncodingEdge *, EncodingEdge *, uintptr_t, PTRSHIFT, hashEncodingEdge, equalsEncodingEdge> HashtableEdge;
typedef Hashset<EncodingNode *, uintptr_t, PTRSHIFT> HashsetEncodingNode;
typedef SetIterator<EncodingNode *, uintptr_t, PTRSHIFT> SetIteratorEncodingNode;
typedef Hashset<EncodingEdge *, uintptr_t, PTRSHIFT> HashsetEncodingEdge;
typedef SetIterator<EncodingEdge *, uintptr_t, PTRSHIFT> SetIteratorEncodingEdge;

typedef Hashtable<EncodingNode *, EncodingSubGraph *, uintptr_t, PTRSHIFT> HashtableNodeToSubGraph;
typedef Hashset<EncodingSubGraph *, uintptr_t, PTRSHIFT> HashsetEncodingSubGraph;
typedef SetIterator<EncodingSubGraph *, uintptr_t, PTRSHIFT> SetIteratorEncodingSubGraph;
#endif
