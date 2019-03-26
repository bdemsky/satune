#include "constraint.h"
#include <string.h>
#include <stdlib.h>
#include "inc_solver.h"
#include "common.h"
#include "qsort.h"
/*
   CNF SAT Conversion Copyright Brian Demsky 2017.
 */


VectorImpl(Edge, Edge, 16)
Edge E_True = {(Node *)(uintptr_t) EDGE_IS_VAR_CONSTANT};
Edge E_False = {(Node *)(uintptr_t) (EDGE_IS_VAR_CONSTANT | NEGATE_EDGE)};
Edge E_BOGUS = {(Node *)0xffff5673};
Edge E_NULL = {(Node *)NULL};

CNF *createCNF() {
	CNF *cnf = (CNF *) ourmalloc(sizeof(CNF));
	cnf->varcount = 1;
	cnf->clausecount = 0;
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
	cnf->clausecount = 0;
	cnf->solveTime = 0;
	cnf->encodeTime = 0;
	cnf->unsat = false;
}

Node *allocNode(NodeType type, uint numEdges, Edge *edges) {
	Node *n = (Node *)ourmalloc(sizeof(Node) + sizeof(Edge) * numEdges);
	n->numVars = 0;
	n->type = type;
	n->numEdges = numEdges;
	memcpy(n->edges, edges, sizeof(Edge) * numEdges);
	return n;
}

Node *allocBaseNode(NodeType type, uint numEdges) {
	Node *n = (Node *)ourmalloc(sizeof(Node) + sizeof(Edge) * numEdges);
	n->numVars = 0;
	n->type = type;
	n->numEdges = numEdges;
	return n;
}

Node *allocResizeNode(uint capacity) {
	Node *n = (Node *)ourmalloc(sizeof(Node) + sizeof(Edge) * capacity);
	n->numVars = 0;
	n->numEdges = 0;
	n->capacity = capacity;
	return n;
}

Edge cloneEdge(Edge e) {
	if (edgeIsVarConst(e))
		return e;
	Node *node = getNodePtrFromEdge(e);
	bool isneg = isNegEdge(e);
	uint numEdges = node->numEdges;
	Node *clone = allocBaseNode(node->type, numEdges);
	for (uint i = 0; i < numEdges; i++) {
		clone->edges[i] = cloneEdge(node->edges[i]);
	}
	return isneg ? constraintNegate((Edge) {clone}) : (Edge) {clone};
}

void freeEdgeRec(Edge e) {
	if (edgeIsVarConst(e))
		return;
	Node *node = getNodePtrFromEdge(e);
	uint numEdges = node->numEdges;
	for (uint i = 0; i < numEdges; i++) {
		freeEdgeRec(node->edges[i]);
	}
	ourfree(node);
}

void freeEdge(Edge e) {
	if (edgeIsVarConst(e))
		return;
	Node *node = getNodePtrFromEdge(e);
	ourfree(node);
}

void freeEdgesRec(uint numEdges, Edge *earray) {
	for (uint i = 0; i < numEdges; i++) {
		Edge e = earray[i];
		freeEdgeRec(e);
	}
}

void freeEdgeCNF(Edge e) {
	Node *node = getNodePtrFromEdge(e);
	uint numEdges = node->numEdges;
	for (uint i = 0; i < numEdges; i++) {
		Edge ec = node->edges[i];
		if (!edgeIsVarConst(ec)) {
			ourfree(ec.node_ptr);
		}
	}
	ourfree(node);
}

void addEdgeToResizeNode(Node **node, Edge e) {
	Node *currnode = *node;
	if (currnode->capacity == currnode->numEdges) {
		Node *newnode = allocResizeNode( currnode->capacity << 1);
		newnode->numVars = currnode->numVars;
		newnode->numEdges = currnode->numEdges;
		memcpy(newnode->edges, currnode->edges, newnode->numEdges * sizeof(Edge));
		ourfree(currnode);
		*node = newnode;
		currnode = newnode;
	}
	currnode->edges[currnode->numEdges++] = e;
}

void mergeFreeNodeToResizeNode(Node **node, Node *innode) {
	Node *currnode = *node;
	uint currEdges = currnode->numEdges;
	uint inEdges = innode->numEdges;

	uint newsize = currEdges + inEdges;
	if (newsize >= currnode->capacity) {
		if (newsize < innode->capacity) {
			//just swap
			innode->numVars = currnode->numVars;
			Node *tmp = innode;
			innode = currnode;
			*node = currnode = tmp;
		} else {
			Node *newnode = allocResizeNode( newsize << 1);
			newnode->numVars = currnode->numVars;
			newnode->numEdges = currnode->numEdges;
			memcpy(newnode->edges, currnode->edges, newnode->numEdges * sizeof(Edge));
			ourfree(currnode);
			*node = newnode;
			currnode = newnode;
		}
	} else {
		if (inEdges > currEdges && newsize < innode->capacity) {
			//just swap
			innode->numVars = currnode->numVars;
			Node *tmp = innode;
			innode = currnode;
			*node = currnode = tmp;
		}
	}
	memcpy(&currnode->edges[currnode->numEdges], innode->edges, innode->numEdges * sizeof(Edge));
	currnode->numEdges += innode->numEdges;
	ourfree(innode);
}

void mergeNodeToResizeNode(Node **node, Node *innode) {
	Node *currnode = *node;
	uint currEdges = currnode->numEdges;
	uint inEdges = innode->numEdges;
	uint newsize = currEdges + inEdges;
	if (newsize >= currnode->capacity) {
		Node *newnode = allocResizeNode( newsize << 1);
		newnode->numVars = currnode->numVars;
		newnode->numEdges = currnode->numEdges;
		memcpy(newnode->edges, currnode->edges, newnode->numEdges * sizeof(Edge));
		ourfree(currnode);
		*node = newnode;
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
	if (numEdges < 200000) {
		Edge edgearray[numEdges];

		for (uint i = 0; i < numEdges; i++) {
			edgearray[i] = constraintNegate(edges[i]);
		}
		Edge eand = constraintAND(cnf, numEdges, edgearray);
		return constraintNegate(eand);
	} else {
		Edge * edgearray=(Edge *)ourmalloc(numEdges*sizeof(Edge));
		
		for (uint i = 0; i < numEdges; i++) {
			edgearray[i] = constraintNegate(edges[i]);
		}
		Edge eand = constraintAND(cnf, numEdges, edgearray);
		ourfree(edgearray);
		return constraintNegate(eand);
	}
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
				freeEdgesRec(numEdges - initindex, &edges[initindex]);
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
			Edge result = constraintNegate(constraintITE(cnf, e0edges[0], e0edges[1], e1edges[1]));
			freeEdge(edges[0]);
			freeEdge(edges[1]);
			return result;
		} else if (sameNodeOppSign(e0edges[0], e1edges[1])) {
			Edge result = constraintNegate(constraintITE(cnf, e0edges[0], e0edges[1], e1edges[0]));
			freeEdge(edges[0]);
			freeEdge(edges[1]);
			return result;
		} else if (sameNodeOppSign(e0edges[1], e1edges[0])) {
			Edge result = constraintNegate(constraintITE(cnf, e0edges[1], e0edges[0], e1edges[1]));
			freeEdge(edges[0]);
			freeEdge(edges[1]);
			return result;
		} else if (sameNodeOppSign(e0edges[1], e1edges[1])) {
			Edge result = constraintNegate(constraintITE(cnf, e0edges[1], e0edges[0], e1edges[0]));
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

Edge disjoinLit(Edge vec, Edge lit) {
	Node *nvec = vec.node_ptr;
	uint nvecedges = nvec->numEdges;

	for (uint i = 0; i < nvecedges; i++) {
		Edge ce = nvec->edges[i];
		if (!edgeIsVarConst(ce)) {
			Node *cne = ce.node_ptr;
			addEdgeToResizeNode(&cne, lit);
			nvec->edges[i] = (Edge) {cne};
		} else {
			Node *clause = allocResizeNode(2);
			addEdgeToResizeNode(&clause, lit);
			addEdgeToResizeNode(&clause, ce);
			nvec->edges[i] = (Edge) {clause};
		}
	}
	nvec->numVars += nvecedges;
	return vec;
}

Edge disjoinAndFree(CNF *cnf, Edge newvec, Edge cnfform) {
	Node *nnewvec = newvec.node_ptr;
	Node *ncnfform = cnfform.node_ptr;
	uint newvecedges = nnewvec->numEdges;
	uint cnfedges = ncnfform->numEdges;
	uint newvecvars = nnewvec->numVars;
	uint cnfvars = ncnfform->numVars;

	if (cnfedges > 3 ||
			((cnfedges * newvecvars + newvecedges * cnfvars) > (cnfedges + newvecedges + newvecvars + cnfvars))) {
		Edge proxyVar = constraintNewVar(cnf);
		if (newvecedges > cnfedges) {
			outputCNFOR(cnf, newvec, constraintNegate(proxyVar));
			freeEdgeCNF(newvec);
			return disjoinLit(cnfform, proxyVar);
		} else {
			outputCNFOR(cnf, cnfform, constraintNegate(proxyVar));
			freeEdgeCNF(cnfform);
			return disjoinLit(newvec, proxyVar);
		}
	}



	if (newvecedges == 1 || cnfedges == 1) {
		if (cnfedges != 1) {
			Node *tmp = nnewvec;
			nnewvec = ncnfform;
			ncnfform = tmp;
			newvecedges = cnfedges;
			cnfedges = 1;
		}
		Edge e = ncnfform->edges[0];
		if (!edgeIsVarConst(e)) {
			Node *n = e.node_ptr;
			for (uint i = 0; i < newvecedges; i++) {
				Edge ce = nnewvec->edges[i];
				if (isNodeEdge(ce)) {
					Node *cne = ce.node_ptr;
					mergeNodeToResizeNode(&cne, n);
					nnewvec->edges[i] = (Edge) {cne};
				} else {
					Node *clause = allocResizeNode(n->numEdges + 1);
					mergeNodeToResizeNode(&clause, n);
					addEdgeToResizeNode(&clause, ce);
					nnewvec->edges[i] = (Edge) {clause};
				}
			}
			nnewvec->numVars += newvecedges * n->numVars;
		} else {
			for (uint i = 0; i < newvecedges; i++) {
				Edge ce = nnewvec->edges[i];
				if (!edgeIsVarConst(ce)) {
					Node *cne = ce.node_ptr;
					addEdgeToResizeNode(&cne, e);
					nnewvec->edges[i] = (Edge) {cne};
				} else {
					Node *clause = allocResizeNode(2);
					addEdgeToResizeNode(&clause, e);
					addEdgeToResizeNode(&clause, ce);
					nnewvec->edges[i] = (Edge) {clause};
				}
			}
			nnewvec->numVars += newvecedges;
		}
		freeEdgeCNF((Edge) {ncnfform});
		return (Edge) {nnewvec};
	}

	Node *result = allocResizeNode(1);

	for (uint i = 0; i < newvecedges; i++) {
		Edge nedge = nnewvec->edges[i];
		uint nSize = isNodeEdge(nedge) ? nedge.node_ptr->numEdges : 1;
		for (uint j = 0; j < cnfedges; j++) {
			Edge cedge = ncnfform->edges[j];
			uint cSize = isNodeEdge(cedge) ? cedge.node_ptr->numEdges : 1;
			if (equalsEdge(cedge, nedge)) {
				addEdgeToResizeNode(&result, cedge);
				result->numVars += cSize;
			} else if (!sameNodeOppSign(nedge, cedge)) {
				Node *clause = allocResizeNode(cSize + nSize);
				if (isNodeEdge(nedge)) {
					mergeNodeToResizeNode(&clause, nedge.node_ptr);
				} else {
					addEdgeToResizeNode(&clause, nedge);
				}
				if (isNodeEdge(cedge)) {
					mergeNodeToResizeNode(&clause, cedge.node_ptr);
				} else {
					addEdgeToResizeNode(&clause, cedge);
				}
				addEdgeToResizeNode(&result, (Edge) {clause});
				result->numVars += clause->numEdges;
			}
			//otherwise skip
		}
	}
	freeEdgeCNF(newvec);
	freeEdgeCNF(cnfform);
	return (Edge) {result};
}

Edge simplifyCNF(CNF *cnf, Edge input) {
	if (edgeIsVarConst(input)) {
		Node *newvec = allocResizeNode(1);
		addEdgeToResizeNode(&newvec, input);
		newvec->numVars = 1;
		return (Edge) {newvec};
	}
	bool negated = isNegEdge(input);
	Node *node = getNodePtrFromEdge(input);
	NodeType type = node->type;
	if (!negated) {
		if (type == NodeType_AND) {
			//AND case
			Node *newvec = allocResizeNode(node->numEdges);
			uint numEdges = node->numEdges;
			for (uint i = 0; i < numEdges; i++) {
				Edge e = simplifyCNF(cnf, node->edges[i]);
				uint enumvars = e.node_ptr->numVars;
				mergeFreeNodeToResizeNode(&newvec, e.node_ptr);
				newvec->numVars += enumvars;
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
			Node *result = thencnf.node_ptr;
			uint elsenumvars = elsecnf.node_ptr->numVars;
			mergeFreeNodeToResizeNode(&result, elsecnf.node_ptr);
			result->numVars += elsenumvars;
			return (Edge) {result};
		}
	} else {
		if (type == NodeType_AND) {
			//OR Case
			uint numEdges = node->numEdges;

			Edge newvec = simplifyCNF(cnf, constraintNegate(node->edges[0]));
			for (uint i = 1; i < numEdges; i++) {
				Edge e = node->edges[i];
				Edge cnfform = simplifyCNF(cnf, constraintNegate(e));
				newvec = disjoinAndFree(cnf, newvec, cnfform);
			}
			return newvec;
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

void addClause(CNF *cnf, uint numliterals, int *literals) {
	cnf->clausecount++;
	addArrayClauseLiteral(cnf->solver, numliterals, literals);
}

void outputCNFOR(CNF *cnf, Edge cnfform, Edge eorvar) {
	Node *andNode = cnfform.node_ptr;
	int orvar = getEdgeVar(eorvar);
	ASSERT(orvar != 0);
	uint numEdges = andNode->numEdges;
	for (uint i = 0; i < numEdges; i++) {
		Edge e = andNode->edges[i];
		if (edgeIsVarConst(e)) {
			int array[2] = {getEdgeVar(e), orvar};
			ASSERT(array[0] != 0);
			addClause(cnf, 2, array);
		} else {
			Node *clause = e.node_ptr;
			uint cnumEdges = clause->numEdges + 1;
			if (cnumEdges > cnf->asize) {
				cnf->asize = cnumEdges << 1;
				ourfree(cnf->array);
				cnf->array = (int *) ourmalloc(sizeof(int) * cnf->asize);
			}
			int *array = cnf->array;
			for (uint j = 0; j < (cnumEdges - 1); j++) {
				array[j] = getEdgeVar(clause->edges[j]);
				ASSERT(array[j] != 0);
			}
			array[cnumEdges - 1] = orvar;
			addClause(cnf, cnumEdges, array);
		}
	}
}

void outputCNF(CNF *cnf, Edge cnfform) {
	Node *andNode = cnfform.node_ptr;
	uint numEdges = andNode->numEdges;
	for (uint i = 0; i < numEdges; i++) {
		Edge e = andNode->edges[i];
		if (edgeIsVarConst(e)) {
			int array[1] = {getEdgeVar(e)};
			ASSERT(array[0] != 0);
			addClause(cnf, 1, array);
		} else {
			Node *clause = e.node_ptr;
			uint cnumEdges = clause->numEdges;
			if (cnumEdges > cnf->asize) {
				cnf->asize = cnumEdges << 1;
				ourfree(cnf->array);
				cnf->array = (int *) ourmalloc(sizeof(int) * cnf->asize);
			}
			int *array = cnf->array;
			for (uint j = 0; j < cnumEdges; j++) {
				array[j] = getEdgeVar(clause->edges[j]);
				ASSERT(array[j] != 0);
			}
			addClause(cnf, cnumEdges, array);
		}
	}
}

void generateProxy(CNF *cnf, Edge expression, Edge proxy, Polarity p) {
	ASSERT(p != P_UNDEFINED);
	if (p == P_TRUE || p == P_BOTHTRUEFALSE) {
		// proxy => expression
		Edge cnfexpr = simplifyCNF(cnf, expression);
		if (p == P_TRUE)
			freeEdgeRec(expression);
		outputCNFOR(cnf, cnfexpr, constraintNegate(proxy));
		freeEdgeCNF(cnfexpr);
	}
	if (p == P_FALSE || p == P_BOTHTRUEFALSE) {
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
	model_print("#Clauses = %u\t#Vars = %u\n", cnf->clausecount, cnf->varcount);
	int result = cnf->unsat ? IS_UNSAT : solve(cnf->solver);
	long long finishTime = getTimeNano();
	cnf->encodeTime = startSolve - startTime;
	model_print("CNF Encode time: %f\n", cnf->encodeTime / 1000000000.0);
	cnf->solveTime = finishTime - startSolve;
	model_print("SAT Solving time: %f\n", cnf->solveTime / 1000000000.0);
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

void generateAddConstraint(CNF *cnf, uint nSum, Edge *sum, uint nVar1, Edge *var1, uint nVar2, Edge *var2) {
	//TO WRITE....
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
