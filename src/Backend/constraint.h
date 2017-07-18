#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include "classlist.h"
#include "vector.h"

#define NEGATE_EDGE 1
#define EDGE_IS_VAR_CONSTANT 2
#define VAR_SHIFT 2
#define EDGE_MASK (NEGATE_EDGE | EDGE_IS_VAR_CONSTANT)

typedef int Literal;

struct Edge;
typedef struct Edge Edge;

struct Node;
typedef struct Node Node;

struct Edge {
	Node * node_ptr;
};

VectorDef(Edge, Edge)

enum NodeType {
	NodeType_AND,
	NodeType_ITE,
	NodeType_IFF
};

typedef enum NodeType NodeType;

struct NodeFlags {
	NodeType type:2;
	int varForced:1;
	int wasExpanded:2;
	int cnfVisitedDown:2;
	int cnfVisitedUp:2;
};

typedef struct NodeFlags NodeFlags;

struct Node {
	NodeFlags flags;
	uint numEdges;
	uint hashCode;
	uint intAnnot[2];
	void * ptrAnnot[2];
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
	bool enableMatching;
	Node ** node_array;
	IncrementalSolver * solver;
	VectorEdge constraints;
	VectorEdge args;
};

typedef struct CNF CNF;

struct CNFExpr;
typedef struct CNFExpr CNFExpr;

static inline bool getExpanded(Node *n, int isNegated) {
	return n->flags.wasExpanded & (1<<isNegated);
}

static inline void setExpanded(Node *n, int isNegated) {
	n->flags.wasExpanded |= (1<<isNegated);
}

static inline Edge constraintNegate(Edge e) {
	if(edgeIsNull(e))
		return e;
	Edge enew = { (Node *) (((uintptr_t) e.node_ptr) ^ NEGATE_EDGE)};
	return enew;
}

static inline bool sameNodeVarEdge(Edge e1, Edge e2) {
	return ! (((uintptr_t) e1.node_ptr ^ (uintptr_t) e2.node_ptr) & (~ (uintptr_t) NEGATE_EDGE));
}

static inline bool sameSignEdge(Edge e1, Edge e2) {
	return !(((uintptr_t) e1.node_ptr ^ (uintptr_t) e2.node_ptr) & NEGATE_EDGE);
}

static inline bool sameNodeOppSign(Edge e1, Edge e2) {
	return (((uintptr_t) e1.node_ptr) ^ ((uintptr_t)e2.node_ptr)) == NEGATE_EDGE;
}

static inline bool isNegEdge(Edge e) {
	return ((uintptr_t)e.node_ptr) & NEGATE_EDGE;
}

static inline bool isPosEdge(Edge e) {
	return !(((uintptr_t)e.node_ptr) & NEGATE_EDGE);
}

static inline bool isNodeEdge(Edge e) {
	return !(((uintptr_t)e.node_ptr) & EDGE_IS_VAR_CONSTANT);
}

static inline bool isNegNodeEdge(Edge e) {
	return (((uintptr_t) e.node_ptr) & (NEGATE_EDGE | EDGE_IS_VAR_CONSTANT)) == NEGATE_EDGE;
}

static inline Node * getNodePtrFromEdge(Edge e) {
	return (Node *) (((uintptr_t) e.node_ptr) & ~((uintptr_t) EDGE_MASK));
}

static inline NodeType getNodeType(Edge e) {
	Node * n=getNodePtrFromEdge(e);
	return n->flags.type;
}

static inline bool equalsEdge(Edge e1, Edge e2) {
	return e1.node_ptr == e2.node_ptr;
}

static inline bool ltEdge(Edge e1, Edge e2) {
	return (uintptr_t) e1.node_ptr < (uintptr_t) e2.node_ptr;
}

static inline uint getNodeSize(Edge e) {
	Node * n=getNodePtrFromEdge(e);
	return n->numEdges;
}

static inline Edge * getEdgeArray(Edge e) {
	Node * n=getNodePtrFromEdge(e);
	return n->edges;
}

static inline Edge getNonNeg(Edge e) {
	Edge enew={(Node *)(((uintptr_t)e.node_ptr)&(~((uintptr_t)NEGATE_EDGE)))};
	return enew;
}

static inline bool edgeIsConst(Edge e) {
	return (((uintptr_t) e.node_ptr) & ~((uintptr_t)NEGATE_EDGE)) == EDGE_IS_VAR_CONSTANT;
}

static inline bool edgeIsNull(Edge e) {
	return e.node_ptr == NULL;
}

static inline bool edgeIsVarConst(Edge e) {
	return ((uintptr_t)e.node_ptr) & EDGE_IS_VAR_CONSTANT;
}

static inline Edge constraintNegateIf(Edge e, bool negate) {
	Edge eret={(Node *)(((uintptr_t)e.node_ptr) ^ negate)};
	return eret;
}

static inline Literal getEdgeVar(Edge e) {
	int val = (int) (((uintptr_t) e.node_ptr) >> VAR_SHIFT);
	return isNegEdge(e) ? -val : val;
}

static inline bool isProxy(CNFExpr *expr) {
	return (bool) (((intptr_t) expr) & 1);
}

static inline Literal getProxy(CNFExpr *expr) {
	return (Literal) (((intptr_t) expr) >> 1);
}

CNF * createCNF();
void deleteCNF(CNF * cnf);

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
static inline Edge constraintXOR(CNF *cnf, Edge left, Edge right) {return constraintNegate(constraintIFF(cnf, left,right));}
Edge constraintITE(CNF * cnf, Edge cond, Edge thenedge, Edge elseedge);
Edge constraintNewVar(CNF *cnf);
void countPass(CNF *cnf);
void countConstraint(CNF *cnf, VectorEdge * stack, Edge e);
void addConstraintCNF(CNF *cnf, Edge constraint);
int solveCNF(CNF *cnf);
bool getValueCNF(CNF *cnf, Edge var);
void printCNF(Edge e);

void convertPass(CNF *cnf, bool backtrackLit);
void convertConstraint(CNF *cnf, VectorEdge *stack, Edge e, bool backtrackLit);
void constrainCNF(CNF * cnf, Literal l, CNFExpr *exp);
void produceCNF(CNF * cnf, Edge e);
CNFExpr * produceConjunction(CNF * cnf, Edge e);
CNFExpr* produceDisjunction(CNF *cnf, Edge e);
bool propagate(CNF *cnf, CNFExpr ** dest, CNFExpr * src, bool negate);
void saveCNF(CNF *cnf, CNFExpr* exp, Edge e, bool sign);
CNFExpr* fillArgs(CNF * cnf, Edge e, bool isNeg, Edge * largestEdge);
Literal introProxy(CNF * cnf, Edge e, CNFExpr* exp, bool isNeg);
void outputCNF(CNF *cnf, CNFExpr *expr);

Edge generateBinaryConstraint(CNF *cnf, uint numvars, Edge * vars, uint value);
Edge generateLTConstraint(CNF *cnf, uint numvars, Edge * vars, uint value);
Edge generateEquivNVConstraint(CNF *cnf, uint numvars, Edge *var1, Edge *var2);

extern Edge E_True;
extern Edge E_False;
extern Edge E_BOGUS;
extern Edge E_NULL;
#endif
