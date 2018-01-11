#include "constraint.h"
#include <string.h>
#include <stdlib.h>
#include "inc_solver.h"
#include "common.h"
#include "qsort.h"
/*
   V2 Copyright (c) 2014 Ben Chambers, Eugene Goldberg, Pete Manolios,
   Vasilis Papavasileiou, Sudarshan Srinivasan, and Daron Vroon.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.  If
   you download or use the software, send email to Pete Manolios
   (pete@ccs.neu.edu) with your name, contact information, and a short
   note describing what you want to use BAT for.  For any reuse or
   distribution, you must make clear to others the license terms of this
   work.

   Contact Pete Manolios if you want any of these conditions waived.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
   C port of CNF SAT Conversion Copyright Brian Demsky 2017.
 */


VectorImpl(Edge, Edge, 16)
Edge E_True = {(Node *)(uintptr_t) EDGE_IS_VAR_CONSTANT};
Edge E_False = {(Node *)(uintptr_t) (EDGE_IS_VAR_CONSTANT | NEGATE_EDGE)};
Edge E_BOGUS = {(Node *)0xffff5673};
Edge E_NULL = {(Node *)NULL};


CNF *createCNF() {
	CNF *cnf = (CNF *) ourmalloc(sizeof(CNF));
	cnf->varcount = 1;
	cnf->solver = allocIncrementalSolver();
	cnf->solveTime = 0;
	cnf->encodeTime = 0;
	cnf->asize = DEFAULT_CNF_ARRAY_SIZE;
	cnf->array = (int *) ourmalloc(sizeof(int) * DEFAULT_CNF_ARRAY_SIZE);
	cnf->unsat = false;
	return cnf;
}

void deleteCNF(CNF *cnf) {
	deleteIncrementalSolver(cnf->solver);
	ourfree(cnf->array);
	ourfree(cnf);
}

void resetCNF(CNF *cnf) {
	resetSolver(cnf->solver);
	cnf->varcount = 1;
	cnf->solveTime = 0;
	cnf->encodeTime = 0;
	cnf->unsat = false;
}

Node *allocNode(NodeType type, uint numEdges, Edge *edges) {
	Node *n = (Node *)ourmalloc(sizeof(Node) + sizeof(Edge) * numEdges);
	n->type = type;
	n->numEdges = numEdges;
	memcpy(n->edges, edges, sizeof(Edge) * numEdges);
	return n;
}

Node *allocBaseNode(NodeType type, uint numEdges) {
	Node *n = (Node *)ourmalloc(sizeof(Node) + sizeof(Edge) * numEdges);
	n->type = type;
	n->numEdges = numEdges;
	return n;
}

Node *allocResizeNode(uint capacity) {
	Node *n = (Node *)ourmalloc(sizeof(Node) + sizeof(Edge) * capacity);
	n->numEdges = 0;
	n->capacity = capacity;
	return n;
}

Edge cloneEdge(Edge e) {
	if (edgeIsVarConst(e))
		return e;
	Node * node = getNodePtrFromEdge(e);
	bool isneg = isNegEdge(e);
	uint numEdges = node->numEdges;
	Node * clone = allocBaseNode(node->type, numEdges);
	for(uint i=0; i < numEdges; i++) {
		clone->edges[i] = cloneEdge(node->edges[i]);
	}
	return isneg ? constraintNegate((Edge) {clone}) : (Edge) {clone};
}

void freeEdgeRec(Edge e) {
	if (edgeIsVarConst(e))
		return;
	Node * node = getNodePtrFromEdge(e);
	uint numEdges = node->numEdges;
	for(uint i=0; i < numEdges; i++) {
		freeEdgeRec(node->edges[i]);
	}
	ourfree(node);
}

void freeEdge(Edge e) {
	if (edgeIsVarConst(e))
		return;
	Node * node = getNodePtrFromEdge(e);
	ourfree(node);
}

void freeEdgesRec(uint numEdges, Edge * earray) {
	for(uint i=0; i < numEdges; i++) {
		Edge e = earray[i];
		freeEdgeRec(e);
	}
}

void freeEdgeCNF(Edge e) {
	Node * node = getNodePtrFromEdge(e);
	uint numEdges = node->numEdges;
	for(uint i=0; i < numEdges; i++) {
		Edge ec = node->edges[i];
		if (!edgeIsVarConst(ec)) {
			ourfree(ec.node_ptr);
		}
	}
	ourfree(node);
}

void addEdgeToResizeNode(Node ** node, Edge e) {
	Node *currnode = *node;
	if (currnode->capacity == currnode->numEdges) {
		Node *newnode = allocResizeNode( currnode->capacity << 1);
		newnode->numEdges = currnode->numEdges;
		memcpy(newnode->edges, currnode->edges, newnode->numEdges * sizeof(Edge));
		ourfree(currnode);
		*node=newnode;
		currnode = newnode;
	}
	currnode->edges[currnode->numEdges++] = e;
}

void mergeFreeNodeToResizeNode(Node **node, Node * innode) {
	Node * currnode = *node;
	uint currEdges = currnode->numEdges;
	uint inEdges = innode->numEdges;
	
	uint newsize = currEdges + inEdges;
	if (newsize >= currnode->capacity) {
		if (newsize < innode->capacity) {
			//just swap
			Node *tmp = innode;
			innode = currnode;
			*node = currnode = tmp;
		} else {
			Node *newnode = allocResizeNode( newsize << 1);
			newnode->numEdges = currnode->numEdges;
			memcpy(newnode->edges, currnode->edges, newnode->numEdges * sizeof(Edge));
			ourfree(currnode);
			*node=newnode;
			currnode = newnode;
		}
	} else {
		if (inEdges > currEdges && newsize < innode->capacity) {
			//just swap
			Node *tmp = innode;
			innode = currnode;
			*node = currnode = tmp;
		}
	}
	memcpy(&currnode->edges[currnode->numEdges], innode->edges, innode->numEdges * sizeof(Edge));
	currnode->numEdges += innode->numEdges;
	ourfree(innode);
}

void mergeNodeToResizeNode(Node **node, Node * innode) {
	Node * currnode = *node;
	uint currEdges = currnode->numEdges;
	uint inEdges = innode->numEdges;
	uint newsize = currEdges + inEdges;
	if (newsize >= currnode->capacity) {
		Node *newnode = allocResizeNode( newsize << 1);
		newnode->numEdges = currnode->numEdges;
		memcpy(newnode->edges, currnode->edges, newnode->numEdges * sizeof(Edge));
		ourfree(currnode);
		*node=newnode;
		currnode = newnode;
	}
	memcpy(&currnode->edges[currnode->numEdges], innode->edges, inEdges * sizeof(Edge));
	currnode->numEdges += inEdges;
}

Edge createNode(NodeType type, uint numEdges, Edge *edges) {
	Edge e = {allocNode(type, numEdges, edges)};
	return e;
}

Edge constraintOR(CNF *cnf, uint numEdges, Edge *edges) {
	Edge edgearray[numEdges];

	for (uint i = 0; i < numEdges; i++) {
		edgearray[i] = constraintNegate(edges[i]);
	}
	Edge eand = constraintAND(cnf, numEdges, edgearray);
	return constraintNegate(eand);
}

Edge constraintOR2(CNF *cnf, Edge left, Edge right) {
	Edge lneg = constraintNegate(left);
	Edge rneg = constraintNegate(right);
	Edge eand = constraintAND2(cnf, lneg, rneg);
	return constraintNegate(eand);
}

int comparefunction(const Edge *e1, const Edge *e2) {
	if (e1->node_ptr == e2->node_ptr)
		return 0;
	if (((uintptr_t)e1->node_ptr) < ((uintptr_t) e2->node_ptr))
		return -1;
	else
		return 1;
}

Edge constraintAND(CNF *cnf, uint numEdges, Edge *edges) {
	ASSERT(numEdges != 0);

	bsdqsort(edges, numEdges, sizeof(Edge), (int (*)(const void *, const void *))comparefunction);
	uint initindex = 0;
	while (initindex < numEdges && equalsEdge(edges[initindex], E_True))
		initindex++;

	uint remainSize = numEdges - initindex;

	if (remainSize == 0)
		return E_True;
	else if (remainSize == 1)
		return edges[initindex];
	else if (equalsEdge(edges[initindex], E_False)) {
		freeEdgesRec(numEdges, edges);
		return E_False;
	}

	/** De-duplicate array */
	uint lowindex = 0;
	edges[lowindex] = edges[initindex++];

	for (; initindex < numEdges; initindex++) {
		Edge e1 = edges[lowindex];
		Edge e2 = edges[initindex];
		if (sameNodeVarEdge(e1, e2)) {
			ASSERT(!isNodeEdge(e1));
			if (!sameSignEdge(e1, e2)) {
				freeEdgesRec(lowindex + 1, edges);
				freeEdgesRec(numEdges-initindex, &edges[initindex]);
				return E_False;
			}
		} else
			edges[++lowindex] = edges[initindex];
	}
	lowindex++;	//Make lowindex look like size

	if (lowindex == 1)
		return edges[0];

	if (lowindex == 2 &&
			isNegNodeEdge(edges[0]) && isNegNodeEdge(edges[1]) &&
			getNodeType(edges[0]) == NodeType_AND &&
			getNodeType(edges[1]) == NodeType_AND &&
			getNodeSize(edges[0]) == 2 &&
			getNodeSize(edges[1]) == 2) {
		Edge *e0edges = getEdgeArray(edges[0]);
		Edge *e1edges = getEdgeArray(edges[1]);
		if (sameNodeOppSign(e0edges[0], e1edges[0])) {
			Edge result=constraintNegate(constraintITE(cnf, e0edges[0], e0edges[1], e1edges[1]));
			freeEdge(edges[0]);
			freeEdge(edges[1]);
			return result;
		} else if (sameNodeOppSign(e0edges[0], e1edges[1])) {
			Edge result=constraintNegate(constraintITE(cnf, e0edges[0], e0edges[1], e1edges[0]));
			freeEdge(edges[0]);
			freeEdge(edges[1]);
			return result;
		} else if (sameNodeOppSign(e0edges[1], e1edges[0])) {
			Edge result=constraintNegate(constraintITE(cnf, e0edges[1], e0edges[0], e1edges[1]));
			freeEdge(edges[0]);
			freeEdge(edges[1]);
			return result;
		} else if (sameNodeOppSign(e0edges[1], e1edges[1])) {
			Edge result=constraintNegate(constraintITE(cnf, e0edges[1], e0edges[0], e1edges[0]));
			freeEdge(edges[0]);
			freeEdge(edges[1]);
			return result;
		}
	}

	return createNode(NodeType_AND, lowindex, edges);
}

Edge constraintAND2(CNF *cnf, Edge left, Edge right) {
	Edge edges[2] = {left, right};
	return constraintAND(cnf, 2, edges);
}

Edge constraintIMPLIES(CNF *cnf, Edge left, Edge right) {
	Edge array[2];
	array[0] = left;
	array[1] = constraintNegate(right);
	Edge eand = constraintAND(cnf, 2, array);
	return constraintNegate(eand);
}

Edge constraintIFF(CNF *cnf, Edge left, Edge right) {
	bool negate = !sameSignEdge(left, right);
	Edge lpos = getNonNeg(left);
	Edge rpos = getNonNeg(right);

	Edge e;
	if (equalsEdge(lpos, rpos)) {
		freeEdgeRec(left);
		freeEdgeRec(right);
		e = E_True;
	} else if (ltEdge(lpos, rpos)) {
		Edge edges[] = {lpos, rpos};
		e = (edgeIsConst(lpos)) ? rpos : createNode(NodeType_IFF, 2, edges);
	} else {
		Edge edges[] = {rpos, lpos};
		e = (edgeIsConst(rpos)) ? lpos : createNode(NodeType_IFF, 2, edges);
	}
	if (negate)
		e = constraintNegate(e);
	return e;
}

Edge constraintITE(CNF *cnf, Edge cond, Edge thenedge, Edge elseedge) {
	if (isNegEdge(cond)) {
		cond = constraintNegate(cond);
		Edge tmp = thenedge;
		thenedge = elseedge;
		elseedge = tmp;
	}

	bool negate = isNegEdge(thenedge);
	if (negate) {
		thenedge = constraintNegate(thenedge);
		elseedge = constraintNegate(elseedge);
	}

	Edge result;
	if (equalsEdge(cond, E_True)) {
		freeEdgeRec(elseedge);
		result = thenedge;
	} else if (equalsEdge(thenedge, E_True) || equalsEdge(cond, thenedge)) {
		Edge array[] = {cond, elseedge};
		result = constraintOR(cnf,  2, array);
	} else if (equalsEdge(elseedge, E_True) || sameNodeOppSign(cond, elseedge)) {
		result = constraintIMPLIES(cnf, cond, thenedge);
	} else if (equalsEdge(elseedge, E_False) || equalsEdge(cond, elseedge)) {
		Edge array[] = {cond, thenedge};
		result = constraintAND(cnf, 2, array);
	} else if (equalsEdge(thenedge, elseedge)) {
		freeEdgeRec(cond);
		result = thenedge;
	} else if (sameNodeOppSign(thenedge, elseedge)) {
		if (ltEdge(cond, thenedge)) {
			Edge array[] = {cond, thenedge};
			result = createNode(NodeType_IFF, 2, array);
		} else {
			Edge array[] = {thenedge, cond};
			result = createNode(NodeType_IFF, 2, array);
		}
	} else {
		Edge edges[] = {cond, thenedge, elseedge};
		result = createNode(NodeType_ITE, 3, edges);
	}
	if (negate)
		result = constraintNegate(result);
	return result;
}

Edge simplifyCNF(CNF *cnf, Edge input) {
	if (edgeIsVarConst(input)) {
		Node *newvec = allocResizeNode(1);
		addEdgeToResizeNode(&newvec, input);
		return (Edge) {newvec};
	}
	bool negated = isNegEdge(input);
	Node * node = getNodePtrFromEdge(input);
	NodeType type = node->type;
	if (!negated) {
		if (type == NodeType_AND) {
			//AND case
			Node *newvec = allocResizeNode(node->numEdges);
			uint numEdges = node->numEdges;
			for(uint i = 0; i < numEdges; i++) {
				Edge e = simplifyCNF(cnf, node->edges[i]);
				mergeFreeNodeToResizeNode(&newvec, e.node_ptr);
			}
			return (Edge) {newvec};
		} else {
			Edge cond = node->edges[0];
			Edge thenedge = node->edges[1];
			Edge elseedge = (type == NodeType_IFF) ? constraintNegate(thenedge) : node->edges[2];
			Edge thenedges[] = {cond, constraintNegate(thenedge)};
			Edge thencons = constraintNegate(createNode(NodeType_AND, 2, thenedges));
			Edge elseedges[] = {constraintNegate(cond), constraintNegate(elseedge)};
			Edge elsecons = constraintNegate(createNode(NodeType_AND, 2, elseedges));
			Edge thencnf = simplifyCNF(cnf, thencons);
			Edge elsecnf = simplifyCNF(cnf, elsecons);
			//free temporary nodes
			ourfree(getNodePtrFromEdge(thencons));
			ourfree(getNodePtrFromEdge(elsecons));
			Node * result = thencnf.node_ptr;
			mergeFreeNodeToResizeNode(&result, elsecnf.node_ptr);
			return (Edge) {result};
		}
	} else {
		if (type == NodeType_AND) {
			//OR Case
			uint numEdges = node->numEdges;
			
			for(uint i = 0; i < numEdges; i++) {
				Edge e = node->edges[i];
				bool enegate = isNegEdge(e);
				if (!edgeIsVarConst(e)) {
					Node * enode = getNodePtrFromEdge(e);
					NodeType etype = enode->type;
					if (enegate) {
						if (etype == NodeType_AND) {
							//OR of AND Case
							uint eNumEdges = enode->numEdges;
							Node * newnode = allocResizeNode(0);
							Node * clause = allocBaseNode(NodeType_AND, numEdges);
							memcpy(clause->edges, node->edges, sizeof(Edge) * i);
							if ((i + 1) < numEdges) {
								memcpy(&clause->edges[i+1], &node->edges[i+1], sizeof(Edge) * (numEdges - i - 1));
							}
										 
							for(uint j = 0; j < eNumEdges; j++) {
								clause->edges[i] = constraintNegate(enode->edges[j]);
								Edge simplify = simplifyCNF(cnf, constraintNegate((Edge){clause}));
								mergeFreeNodeToResizeNode(&newnode, simplify.node_ptr);
							}
							ourfree(clause);
							return (Edge) {newnode};
						} else {
							//OR of IFF or ITE
							Edge cond = enode->edges[0];
							Edge thenedge = enode->edges[1];
							Edge elseedge = (enode->type == NodeType_IFF) ? constraintNegate(thenedge) : enode->edges[2];
							Edge thenedges[] = {cond, constraintNegate(thenedge)};
							Edge thencons = constraintNegate(createNode(NodeType_AND, 2, thenedges));
							Edge elseedges[] = {constraintNegate(cond), constraintNegate(elseedge)};
							Edge elsecons = constraintNegate(createNode(NodeType_AND, 2, elseedges));

							//OR of AND Case
							Node * newnode = allocResizeNode(0);
							Node * clause = allocBaseNode(NodeType_AND, numEdges);
							memcpy(clause->edges, node->edges, sizeof(Edge) * i);
							if ((i + 1) < numEdges) {
								memcpy(&clause->edges[i+1], &node->edges[i+1], sizeof(Edge) * (numEdges - i - 1));
							}

							clause->edges[i] = constraintNegate(thencons);
							Edge simplify = simplifyCNF(cnf, constraintNegate((Edge){clause}));
							mergeFreeNodeToResizeNode(&newnode, simplify.node_ptr);
							
							clause->edges[i] = constraintNegate(elsecons);
							simplify = simplifyCNF(cnf, constraintNegate((Edge){clause}));
							mergeFreeNodeToResizeNode(&newnode, simplify.node_ptr);

							//free temporary nodes
							ourfree(getNodePtrFromEdge(thencons));
							ourfree(getNodePtrFromEdge(elsecons));
							ourfree(clause);
							return (Edge) {newnode};
						}
					} else {
						if (etype == NodeType_AND) {
							//OR of OR Case
							uint eNumEdges = enode->numEdges;
							Node * clause = allocBaseNode(NodeType_AND, eNumEdges + numEdges - 1);
							memcpy(clause->edges, node->edges, sizeof(Edge) * i);
							if ((i + 1) < numEdges) {
								memcpy(&clause->edges[i], &node->edges[i+1], sizeof(Edge) * (numEdges - i - 1));
							}
							memcpy(&clause->edges[numEdges-1], enode->edges, sizeof(Edge) * eNumEdges);
							Edge eclause = {clause};
							Edge result = simplifyCNF(cnf, constraintNegate(eclause));
							ourfree(clause);
							return result;
						} else {
							//OR of !(IFF or ITE)
							Edge cond = node->edges[0];
							Edge thenedge = node->edges[1];
							Edge elseedge = (type == NodeType_IFF) ? constraintNegate(thenedge) : node->edges[2];
							Edge thenedges[] = {cond, constraintNegate(thenedge)};
							Edge thencons = createNode(NodeType_AND, 2, thenedges);
							Edge elseedges[] = {constraintNegate(cond), constraintNegate(elseedge)};
							Edge elsecons = createNode(NodeType_AND, 2, elseedges);


							Node * clause = allocBaseNode(NodeType_AND, numEdges + 1);
							memcpy(clause->edges, node->edges, sizeof(Edge) * i);
							if ((i + 1) < numEdges) {
								memcpy(&clause->edges[i], &node->edges[i+1], sizeof(Edge) * (numEdges - i - 1));
							}
							clause->edges[numEdges-1] = constraintNegate(thencons);
							clause->edges[numEdges] = constraintNegate(elsecons);
							Edge result = simplifyCNF(cnf, constraintNegate((Edge) {clause}));
							//free temporary nodes
							ourfree(getNodePtrFromEdge(thencons));
							ourfree(getNodePtrFromEdge(elsecons));
							ourfree(clause);
							return result;
						}
					}
				}
			}
			
			Node *newvec = allocResizeNode(numEdges);
			for(uint i=0; i < numEdges; i++) {
				addEdgeToResizeNode(&newvec, constraintNegate(node->edges[i]));
			}
			Node * result = allocResizeNode(1);
			addEdgeToResizeNode(&result, (Edge){newvec});
			return (Edge) {result};
		} else {
			Edge cond = node->edges[0];
			Edge thenedge = node->edges[1];
			Edge elseedge = (type == NodeType_IFF) ? constraintNegate(thenedge) : node->edges[2];


			Edge thenedges[] = {cond, constraintNegate(thenedge)};
			Edge thencons = createNode(NodeType_AND, 2, thenedges);
			Edge elseedges[] = {constraintNegate(cond), constraintNegate(elseedge)};
			Edge elsecons = createNode(NodeType_AND, 2, elseedges);
			
			Edge combinededges[] = {constraintNegate(thencons), constraintNegate(elsecons)};
			Edge combined = constraintNegate(createNode(NodeType_AND, 2, combinededges));
			Edge result = simplifyCNF(cnf, combined);
			//free temporary nodes
			ourfree(getNodePtrFromEdge(thencons));
			ourfree(getNodePtrFromEdge(elsecons));
			ourfree(getNodePtrFromEdge(combined));
			return result;
		}
	}
}

void outputCNFOR(CNF *cnf, Edge cnfform, Edge eorvar) {
	Node * andNode = cnfform.node_ptr;
	int orvar = getEdgeVar(eorvar);
	ASSERT(orvar != 0);
	uint numEdges = andNode->numEdges;
	for(uint i=0; i < numEdges; i++) {
		Edge e = andNode->edges[i];
		if (edgeIsVarConst(e)) {
			int array[2] = {getEdgeVar(e), orvar};
			ASSERT(array[0] != 0);
			addArrayClauseLiteral(cnf->solver, 2, array);
		} else {
			Node * clause = e.node_ptr;
			uint cnumEdges = clause->numEdges + 1;
			if (cnumEdges > cnf->asize) {
				cnf->asize = cnumEdges << 1;
				ourfree(cnf->array);
				cnf->array = (int *) ourmalloc(sizeof(int) * cnf->asize);
			}
			int * array = cnf->array;
			for(uint j=0; j < (cnumEdges - 1); j++) {
				array[j] = getEdgeVar(clause->edges[j]);
				ASSERT(array[j] != 0);
			}
			array[cnumEdges - 1] = orvar;
			addArrayClauseLiteral(cnf->solver, cnumEdges, array);
		}
	}
}


void outputCNF(CNF *cnf, Edge cnfform) {
	Node * andNode = cnfform.node_ptr;
	uint numEdges = andNode->numEdges;
	for(uint i=0; i < numEdges; i++) {
		Edge e = andNode->edges[i];
		if (edgeIsVarConst(e)) {
			int array[1] = {getEdgeVar(e)};
			ASSERT(array[0] != 0);
			addArrayClauseLiteral(cnf->solver, 1, array);
		} else {
			Node * clause = e.node_ptr;
			uint cnumEdges = clause->numEdges;
			if (cnumEdges > cnf->asize) {
				cnf->asize = cnumEdges << 1;
				ourfree(cnf->array);
				cnf->array = (int *) ourmalloc(sizeof(int) * cnf->asize);
			}
			int * array = cnf->array;
			for(uint j=0; j < cnumEdges; j++) {
				array[j] = getEdgeVar(clause->edges[j]);
				ASSERT(array[j] != 0);
			}
			addArrayClauseLiteral(cnf->solver, cnumEdges, array);
		}
	}
}

void generateProxy(CNF *cnf, Edge expression, Edge proxy, Polarity p) {
	if (P_TRUE || P_BOTHTRUEFALSE) {
		// proxy => expression
		Edge cnfexpr = simplifyCNF(cnf, expression);
		freeEdgeRec(expression);
		outputCNFOR(cnf, cnfexpr, constraintNegate(proxy));
		freeEdgeCNF(cnfexpr);
	}
	if (P_FALSE || P_BOTHTRUEFALSE) {
		// expression => proxy
		Edge cnfnegexpr = simplifyCNF(cnf, constraintNegate(expression));
		freeEdgeRec(expression);
		outputCNFOR(cnf, cnfnegexpr, proxy);
		freeEdgeCNF(cnfnegexpr);
	}
}

void addConstraintCNF(CNF *cnf, Edge constraint) {
	if (equalsEdge(constraint, E_True)) {
		return;
	} else if (equalsEdge(constraint, E_False)) {
		cnf->unsat = true;
		return;
	}
	if (cnf->unsat) {
		freeEdgeRec(constraint);
		return;
	}

#if 0
	model_print("****SATC_ADDING NEW Constraint*****\n");
	printCNF(constraint);
	model_print("\n******************************\n");
#endif
	
	Edge cnfform = simplifyCNF(cnf, constraint);
	freeEdgeRec(constraint);
	outputCNF(cnf, cnfform);
	freeEdgeCNF(cnfform);
}

Edge constraintNewVar(CNF *cnf) {
	uint varnum = cnf->varcount++;
	Edge e = {(Node *) ((((uintptr_t)varnum) << VAR_SHIFT) | EDGE_IS_VAR_CONSTANT) };
	return e;
}

int solveCNF(CNF *cnf) {
	long long startTime = getTimeNano();
	finishedClauses(cnf->solver);
	long long startSolve = getTimeNano();
	int result = cnf->unsat ? IS_UNSAT : solve(cnf->solver);
	long long finishTime = getTimeNano();
	cnf->encodeTime = startSolve - startTime;
	model_print("CNF Encode time: %f\n", cnf->encodeTime / 1000000000.0);
	cnf->solveTime = finishTime - startSolve;
	model_print("Solve time: %f\n", cnf->solveTime / 1000000000.0);
	return result;
}

bool getValueCNF(CNF *cnf, Edge var) {
	Literal l = getEdgeVar(var);
	bool isneg = (l < 0);
	l = abs(l);
	return isneg ^ getValueSolver(cnf->solver, l);
}

void printCNF(Edge e) {
	if (edgeIsVarConst(e)) {
		Literal l = getEdgeVar(e);
		model_print ("%d", l);
		return;
	}
	bool isNeg = isNegEdge(e);
	if (edgeIsConst(e)) {
		if (isNeg)
			model_print("T");
		else
			model_print("F");
		return;
	}
	Node *n = getNodePtrFromEdge(e);
	if (isNeg) {
		//Pretty print things that are equivalent to OR's
		if (getNodeType(e) == NodeType_AND) {
			model_print("or(");
			for (uint i = 0; i < n->numEdges; i++) {
				Edge e = n->edges[i];
				if (i != 0)
					model_print(" ");
				printCNF(constraintNegate(e));
			}
			model_print(")");
			return;
		}

		model_print("!");
	}
	switch (getNodeType(e)) {
	case NodeType_AND:
		model_print("and");
		break;
	case NodeType_ITE:
		model_print("ite");
		break;
	case NodeType_IFF:
		model_print("iff");
		break;
	}
	model_print("(");
	for (uint i = 0; i < n->numEdges; i++) {
		Edge e = n->edges[i];
		if (i != 0)
			model_print(" ");
		printCNF(e);
	}
	model_print(")");
}

Edge generateBinaryConstraint(CNF *cnf, uint numvars, Edge *vars, uint value) {
	Edge carray[numvars];
	for (uint j = 0; j < numvars; j++) {
		carray[j] = ((value & 1) == 1) ? vars[j] : constraintNegate(vars[j]);
		value = value >> 1;
	}

	return constraintAND(cnf, numvars, carray);
}

/** Generates a constraint to ensure that all encodings are less than value */
Edge generateLTValueConstraint(CNF *cnf, uint numvars, Edge *vars, uint value) {
	Edge orarray[numvars];
	Edge andarray[numvars];
	uint andi = 0;

	while (true) {
		uint val = value;
		uint ori = 0;
		for (uint j = 0; j < numvars; j++) {
			if ((val & 1) == 1)
				orarray[ori++] = constraintNegate(vars[j]);
			val = val >> 1;
		}
		//no ones to flip, so bail now...
		if (ori == 0) {
			return constraintAND(cnf, andi, andarray);
		}
		andarray[andi++] = constraintOR(cnf, ori, orarray);

		value = value + (1 << (__builtin_ctz(value)));
		//flip the last one
	}
}

Edge generateEquivNVConstraint(CNF *cnf, uint numvars, Edge *var1, Edge *var2) {
	if (numvars == 0)
		return E_True;
	Edge array[numvars];
	for (uint i = 0; i < numvars; i++) {
		array[i] = constraintIFF(cnf, var1[i], var2[i]);
	}
	return constraintAND(cnf, numvars, array);
}

Edge generateLTConstraint(CNF *cnf, uint numvars, Edge *var1, Edge *var2) {
	if (numvars == 0 )
		return E_False;
	Edge result = constraintAND2(cnf, constraintNegate( var1[0]), var2[0]);
	for (uint i = 1; i < numvars; i++) {
		Edge lt = constraintAND2(cnf, constraintNegate( var1[i]), var2[i]);
		Edge eq = constraintAND2(cnf, constraintIFF(cnf, var1[i], var2[i]), result);
		result = constraintOR2(cnf, lt, eq);
	}
	return result;
}

Edge generateLTEConstraint(CNF *cnf, uint numvars, Edge *var1, Edge *var2) {
	if (numvars == 0 )
		return E_True;
	Edge result = constraintIMPLIES(cnf, var1[0], var2[0]);
	for (uint i = 1; i < numvars; i++) {
		Edge lt = constraintAND2(cnf, constraintNegate( var1[i]), var2[i]);
		Edge eq = constraintAND2(cnf, constraintIFF(cnf, var1[i], var2[i]), result);
		result = constraintOR2(cnf, lt, eq);
	}
	return result;
}
