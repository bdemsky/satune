#ifndef NODEEDGE_H
#define NODEEDGE_H
#include "classlist.h"

#define NEGATE_EDGE 1
#define EDGE_IS_VAR_CONSTANT 2
#define VAR_SHIFT 2

struct Edge;
typedef struct Edge Edge;

struct Node;
typedef struct Node Node;

struct Edge {
	Node * node_ptr;
};

enum NodeType {
	NodeType_AND,
	NodeType_ITE,
	NodeType_IFF
};

typedef enum NodeType NodeType;

struct NodeFlags {
	NodeType type:2;
};

typedef struct NodeFlags NodeFlags;

struct Node {
	NodeFlags flags;
	uint numEdges;
	uint hashCode;
	Edge edges[];
};

#define DEFAULT_CNF_ARRAY_SIZE 256
#define LOAD_FACTOR 0.25

struct CNF {
	uint varcount;
	uint capacity;
	uint size;
	uint mask;
	uint maxsize;
	Node ** node_array;
};

typedef struct CNF CNF;

static inline Edge constraintNegate(Edge e) {
	Edge enew = { (Node *) (((uintptr_t) e.node_ptr) ^ NEGATE_EDGE)};
	return enew;
}

uint hashNode(NodeType type, uint numEdges, Edge * edges);
Node * allocNode(NodeType type, uint numEdges, Edge * edges, uint hashCode);
bool compareNodes(Node * node, NodeType type, uint numEdges, Edge *edges);
Edge create(CNF *cnf, NodeType type, uint numEdges, Edge * edges);
Edge constraintOR(CNF * cnf, uint numEdges, Edge *edges);
Edge constraintAND(CNF * cnf, uint numEdges, Edge * edges);
Edge constraintOR2(CNF * cnf, Edge left, Edge right);
Edge constraintAND2(CNF * cnf, Edge left, Edge right);
Edge constraintIMPLIES(CNF * cnf, Edge left, Edge right);
Edge constraintIFF(CNF * cnf, Edge left, Edge right);
Edge constraintITE(CNF * cnf, Edge cond, Edge thenedge, Edge elseedge);
Edge constraintNewVar(CNF *cnf);

Edge E_True={(Node *)(uintptr_t) EDGE_IS_VAR_CONSTANT};
Edge E_False={(Node *)(uintptr_t) (EDGE_IS_VAR_CONSTANT | NEGATE_EDGE)};
#endif
