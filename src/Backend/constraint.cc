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
	return cnf;
}

void deleteCNF(CNF *cnf) {
	deleteIncrementalSolver(cnf->solver);
	ourfree(cnf);
}

void resetCNF(CNF *cnf) {
	resetSolver(cnf->solver);
	cnf->varcount = 1;
	cnf->solveTime = 0;
	cnf->encodeTime = 0;
}

Node *allocNode(NodeType type, uint numEdges, Edge *edges) {
	Node *n = (Node *)ourmalloc(sizeof(Node) + sizeof(Edge) * numEdges);
	memcpy(n->edges, edges, sizeof(Edge) * numEdges);
	n->numEdges = numEdges;
	return n;
}

Edge createNode(CNF *cnf, NodeType type, uint numEdges, Edge *edges) {
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
	else if (equalsEdge(edges[initindex], E_False))
		return E_False;

	/** De-duplicate array */
	uint lowindex = 0;
	edges[lowindex] = edges[initindex++];

	for (; initindex < numEdges; initindex++) {
		Edge e1 = edges[lowindex];
		Edge e2 = edges[initindex];
		if (sameNodeVarEdge(e1, e2)) {
			if (!sameSignEdge(e1, e2)) {
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
			return constraintNegate(constraintITE(cnf, e0edges[0], e0edges[1], e1edges[1]));
		} else if (sameNodeOppSign(e0edges[0], e1edges[1])) {
			return constraintNegate(constraintITE(cnf, e0edges[0], e0edges[1], e1edges[0]));
		} else if (sameNodeOppSign(e0edges[1], e1edges[0])) {
			return constraintNegate(constraintITE(cnf, e0edges[1], e0edges[0], e1edges[1]));
		} else if (sameNodeOppSign(e0edges[1], e1edges[1])) {
			return constraintNegate(constraintITE(cnf, e0edges[1], e0edges[0], e1edges[0]));
		}
	}

	return createNode(cnf, NodeType_AND, lowindex, edges);
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
		e = E_True;
	} else if (ltEdge(lpos, rpos)) {
		Edge edges[] = {lpos, rpos};
		e = (edgeIsConst(lpos)) ? rpos : createNode(cnf, NodeType_IFF, 2, edges);
	} else {
		Edge edges[] = {rpos, lpos};
		e = (edgeIsConst(rpos)) ? lpos : createNode(cnf, NodeType_IFF, 2, edges);
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
		result = thenedge;
	} else if (equalsEdge(thenedge, E_True) || equalsEdge(cond, thenedge)) {
		Edge array[] = {cond, elseedge};
		result = constraintOR(cnf,  2, array);
	} else if (equalsEdge(elseedge, E_True) || sameNodeOppSign(cond, elseedge)) {
		result = constraintIMPLIES(cnf, cond, thenedge);
	} else if (equalsEdge(thenedge, E_False) || equalsEdge(cond, elseedge)) {
		Edge array[] = {cond, thenedge};
		result = constraintAND(cnf, 2, array);
	} else if (equalsEdge(thenedge, elseedge)) {
		result = thenedge;
	} else if (sameNodeOppSign(thenedge, elseedge)) {
		if (ltEdge(cond, thenedge)) {
			Edge array[] = {cond, thenedge};
			result = createNode(cnf, NodeType_IFF, 2, array);
		} else {
			Edge array[] = {thenedge, cond};
			result = createNode(cnf, NodeType_IFF, 2, array);
		}
	} else {
		Edge edges[] = {cond, thenedge, elseedge};
		result = createNode(cnf, NodeType_ITE, 3, edges);
	}
	if (negate)
		result = constraintNegate(result);
	return result;
}

void addConstraintCNF(CNF *cnf, Edge constraint) {
	//	pushVectorEdge(&cnf->constraints, constraint);
#ifdef CONFIG_DEBUG
	model_print("****SATC_ADDING NEW Constraint*****\n");
	printCNF(constraint);
	model_print("\n******************************\n");
#endif
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
	int result = solve(cnf->solver);
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
