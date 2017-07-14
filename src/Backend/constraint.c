#include "constraint.h"
#include <string.h>
#include <stdlib.h>
#include "inc_solver.h"
#include "cnfexpr.h"
#include "common.h"
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
Edge E_True={(Node *)(uintptr_t) EDGE_IS_VAR_CONSTANT};
Edge E_False={(Node *)(uintptr_t) (EDGE_IS_VAR_CONSTANT | NEGATE_EDGE)};
Edge E_BOGUS={(Node *)0x12345673};
Edge E_NULL={(Node *)NULL};


CNF * createCNF() {
	CNF * cnf=ourmalloc(sizeof(CNF));
	cnf->varcount=1;
	cnf->capacity=DEFAULT_CNF_ARRAY_SIZE;
	cnf->mask=cnf->capacity-1;
	cnf->node_array=ourcalloc(1, sizeof(Node *)*cnf->capacity);
	cnf->size=0;
	cnf->maxsize=(uint)(((double)cnf->capacity)*LOAD_FACTOR);
	cnf->enableMatching=true;
	initDefVectorEdge(& cnf->constraints);
	initDefVectorEdge(& cnf->args);
	cnf->solver=allocIncrementalSolver();
	return cnf;
}

void deleteCNF(CNF * cnf) {
	for(uint i=0;i<cnf->capacity;i++) {
		Node *n=cnf->node_array[i];
		if (n!=NULL)
			ourfree(n);
	}
	deleteVectorArrayEdge(& cnf->constraints);
	deleteVectorArrayEdge(& cnf->args);
	deleteIncrementalSolver(cnf->solver);
	ourfree(cnf->node_array);
	ourfree(cnf);
}

void resizeCNF(CNF *cnf, uint newCapacity) {
	Node **old_array=cnf->node_array;
	Node **new_array=ourcalloc(1, sizeof(Node *)*newCapacity);
	uint oldCapacity=cnf->capacity;
	uint newMask=newCapacity-1;
	for(uint i=0;i<oldCapacity;i++) {
		Node *n=old_array[i];
		uint hashCode=n->hashCode;
		uint newindex=hashCode & newMask;
		for(;;newindex=(newindex+1) & newMask) {
			if (new_array[newindex] == NULL) {
				new_array[newindex]=n;
				break;
			}
		}
	}
	ourfree(old_array);
	cnf->node_array=new_array;
	cnf->capacity=newCapacity;
	cnf->maxsize=(uint)(((double)cnf->capacity)*LOAD_FACTOR);
	cnf->mask=newMask;
}

Node * allocNode(NodeType type, uint numEdges, Edge * edges, uint hashcode) {
	Node *n=(Node *)ourmalloc(sizeof(Node)+sizeof(Edge)*numEdges);
	memcpy(n->edges, edges, sizeof(Edge)*numEdges);
	n->flags.type=type;
	n->flags.wasExpanded=0;
	n->flags.cnfVisitedDown=0;
	n->flags.cnfVisitedUp=0;
	n->flags.varForced=0;
	n->numEdges=numEdges;
	n->hashCode=hashcode;
	n->intAnnot[0]=0;n->intAnnot[1]=0;
	n->ptrAnnot[0]=NULL;n->ptrAnnot[1]=NULL;
	return n;
}

Edge createNode(CNF *cnf, NodeType type, uint numEdges, Edge * edges) {
	if (cnf->size > cnf->maxsize) {
		resizeCNF(cnf, cnf->capacity << 1);
	}
	uint hashvalue=hashNode(type, numEdges, edges);
	uint mask=cnf->mask;
	uint index=hashvalue & mask;
	Node **n_ptr;
	for(;;index=(index+1)&mask) {
		n_ptr=&cnf->node_array[index];
		if (*n_ptr!=NULL) {
			if ((*n_ptr)->hashCode==hashvalue) {
				if (compareNodes(*n_ptr, type, numEdges, edges)) {
					Edge e={*n_ptr};
					return e;
				}
			}
		} else {
			break;
		}
	}
	*n_ptr=allocNode(type, numEdges, edges, hashvalue);
	Edge e={*n_ptr};
	return e;
}

uint hashNode(NodeType type, uint numEdges, Edge * edges) {
	uint hashvalue=type ^ numEdges;
	for(uint i=0;i<numEdges;i++) {
		hashvalue ^= (uint) ((uintptr_t) edges[i].node_ptr);
		hashvalue = (hashvalue << 3) | (hashvalue >> 29); //rotate left by 3 bits
	}
	return (uint) hashvalue;
}

bool compareNodes(Node * node, NodeType type, uint numEdges, Edge *edges) {
	if (node->flags.type!=type || node->numEdges != numEdges)
		return false;
	Edge *nodeedges=node->edges;
	for(uint i=0;i<numEdges;i++) {
		if (!equalsEdge(nodeedges[i], edges[i]))
			return false;
	}
	return true;
}

Edge constraintOR(CNF * cnf, uint numEdges, Edge *edges) {
	Edge edgearray[numEdges];
	
	for(uint i=0; i<numEdges; i++) {
		edgearray[i]=constraintNegate(edges[i]);
	}
	Edge eand=constraintAND(cnf, numEdges, edgearray);
	return constraintNegate(eand);
}

Edge constraintOR2(CNF * cnf, Edge left, Edge right) {
	Edge lneg=constraintNegate(left);
	Edge rneg=constraintNegate(right);
	Edge eand=constraintAND2(cnf, lneg, rneg);
	return constraintNegate(eand);
}

int comparefunction(const Edge * e1, const Edge * e2) {
	return ((uintptr_t)e1->node_ptr)-((uintptr_t)e2->node_ptr);
}

Edge constraintAND(CNF * cnf, uint numEdges, Edge * edges) {
	ASSERT(numEdges!=0);
	qsort(edges, numEdges, sizeof(Edge), (int (*)(const void *, const void *)) comparefunction);
	int initindex=0;
	while(initindex<numEdges && equalsEdge(edges[initindex], E_True))
		initindex++;

	uint remainSize=numEdges-initindex;

	if (remainSize == 0)
		return E_True;
	else if (remainSize == 1)
		return edges[initindex];
	else if (equalsEdge(edges[initindex], E_False))
		return E_False;

	/** De-duplicate array */
	uint lowindex=0;
	edges[lowindex]=edges[initindex++];

	for(;initindex<numEdges;initindex++) {
		Edge e1=edges[lowindex];
		Edge e2=edges[initindex];
		if (sameNodeVarEdge(e1, e2)) {
			if (!sameSignEdge(e1, e2)) {
				return E_False;
			}
		} else
			edges[++lowindex]=edges[initindex];
	}
	lowindex++; //Make lowindex look like size

	if (lowindex==1)
		return edges[0];

	if (cnf->enableMatching && lowindex==2 &&
			isNegNodeEdge(edges[0]) && isNegNodeEdge(edges[1]) &&
			getNodeType(edges[0]) == NodeType_AND &&
			getNodeType(edges[1]) == NodeType_AND &&
			getNodeSize(edges[0]) == 2 &&
			getNodeSize(edges[1]) == 2) {
		Edge * e0edges=getEdgeArray(edges[0]);
		Edge * e1edges=getEdgeArray(edges[1]);
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

Edge constraintAND2(CNF * cnf, Edge left, Edge right) {
	Edge edges[2]={left, right};
	return constraintAND(cnf, 2, edges);
}

Edge constraintIMPLIES(CNF * cnf, Edge left, Edge right) {
	Edge array[2];
	array[0]=left;
	array[1]=constraintNegate(right);
	Edge eand=constraintAND(cnf, 2, array);
	return constraintNegate(eand);
}

Edge constraintIFF(CNF * cnf, Edge left, Edge right) {
	bool negate=!sameSignEdge(left, right);
	Edge lpos=getNonNeg(left);
	Edge rpos=getNonNeg(right);

	Edge e;
	if (equalsEdge(lpos, rpos)) {
		e=E_True;
	} else if (ltEdge(lpos, rpos)) {
		Edge edges[]={lpos, rpos};
		e=(edgeIsConst(lpos)) ? rpos : createNode(cnf, NodeType_IFF, 2, edges);
	} else {
		Edge edges[]={rpos, lpos};
		e=(edgeIsConst(rpos)) ? lpos : createNode(cnf, NodeType_IFF, 2, edges);
	}
	if (negate)
		e=constraintNegate(e);
	return e;
}

Edge constraintITE(CNF * cnf, Edge cond, Edge thenedge, Edge elseedge) {
	if (isNegEdge(cond)) {
		cond=constraintNegate(cond);
		Edge tmp=thenedge;
		thenedge=elseedge;
		elseedge=tmp;
	}
	
	bool negate = isNegEdge(thenedge);
	if (negate) {
		thenedge=constraintNegate(thenedge);
		elseedge=constraintNegate(elseedge);
	}

	Edge result;
	if (equalsEdge(cond, E_True)) {
		result=thenedge;
	} else if (equalsEdge(thenedge, E_True) || equalsEdge(cond, thenedge)) {
		result=constraintOR(cnf,  2, (Edge[]) {cond, elseedge});
	}	else if (equalsEdge(elseedge, E_True) || sameNodeOppSign(cond, elseedge)) {
		result=constraintIMPLIES(cnf, cond, thenedge);
	} else if (equalsEdge(thenedge, E_False) || equalsEdge(cond, elseedge)) {
		result=constraintAND(cnf, 2, (Edge[]) {cond, thenedge});
	} else if (equalsEdge(thenedge, elseedge)) {
		result=thenedge;
	} else if (sameNodeOppSign(thenedge, elseedge)) {
		if (ltEdge(cond, thenedge)) {
			result=createNode(cnf, NodeType_IFF, 2, (Edge[]) {cond, thenedge});
		} else {
			result=createNode(cnf, NodeType_IFF, 2, (Edge[]) {thenedge, cond});
		}
	} else {
		Edge edges[]={cond, thenedge, elseedge};
		result=createNode(cnf, NodeType_ITE, 3, edges);
	}
	if (negate)
		result=constraintNegate(result);
	return result;
}

void addConstraintCNF(CNF *cnf, Edge constraint) {
	pushVectorEdge(&cnf->constraints, constraint);
	model_print("****ADDING NEW Constraint*****\n");
	printCNF(constraint);
	model_print("\n******************************\n");
}

Edge constraintNewVar(CNF *cnf) {
	uint varnum=cnf->varcount++;
	Edge e={(Node *) ((((uintptr_t)varnum) << VAR_SHIFT) | EDGE_IS_VAR_CONSTANT) };
	return e;
}

int solveCNF(CNF *cnf) {
	countPass(cnf);
	convertPass(cnf, false);
	finishedClauses(cnf->solver);
	return solve(cnf->solver);
}

bool getValueCNF(CNF *cnf, Edge var) {
	Literal l=getEdgeVar(var);
	bool isneg=(l<0);
	l=abs(l);
	return isneg ^ getValueSolver(cnf->solver, l);
}

void countPass(CNF *cnf) {
	uint numConstraints=getSizeVectorEdge(&cnf->constraints);
	VectorEdge *ve=allocDefVectorEdge();
	for(uint i=0; i<numConstraints;i++) {
		countConstraint(cnf, ve, getVectorEdge(&cnf->constraints, i));
	}
	deleteVectorEdge(ve);
}

void countConstraint(CNF *cnf, VectorEdge *stack, Edge eroot) {
	//Skip constants and variables...
	if (edgeIsVarConst(eroot))
		return;

	clearVectorEdge(stack);pushVectorEdge(stack, eroot);

	bool isMatching=cnf->enableMatching;
	
	while(getSizeVectorEdge(stack) != 0) {
		Edge e=lastVectorEdge(stack); popVectorEdge(stack);
		bool polarity=isNegEdge(e);
		Node *n=getNodePtrFromEdge(e);
		if (getExpanded(n,  polarity)) {
			if (n->flags.type == NodeType_IFF ||
					n->flags.type == NodeType_ITE) {
				Edge pExp={n->ptrAnnot[polarity]};
				getNodePtrFromEdge(pExp)->intAnnot[0]++;
			} else {
				n->intAnnot[polarity]++;
			}
		} else {
			setExpanded(n, polarity);

			if (n->flags.type == NodeType_ITE||
					n->flags.type == NodeType_IFF) {
				n->intAnnot[polarity]=0;
				Edge cond=n->edges[0];
				Edge thenedge=n->edges[1];
				Edge elseedge=n->flags.type == NodeType_IFF? constraintNegate(thenedge): n->edges[2];
				thenedge=constraintNegateIf(thenedge, !polarity);
				elseedge=constraintNegateIf(elseedge, !polarity);
				thenedge=constraintAND2(cnf, cond, thenedge);
				cond=constraintNegate(cond);
				elseedge=constraintAND2(cnf, cond, elseedge);
				thenedge=constraintNegate(thenedge);
				elseedge=constraintNegate(elseedge);
				cnf->enableMatching=false;
				Edge succ1=constraintAND2(cnf, thenedge, elseedge);
				n->ptrAnnot[polarity]=succ1.node_ptr;
				cnf->enableMatching=isMatching;
				pushVectorEdge(stack, succ1);
				if (getExpanded(n, !polarity)) {
					Edge succ2={(Node *)n->ptrAnnot[!polarity]};
					Node *n1=getNodePtrFromEdge(succ1);
					Node *n2=getNodePtrFromEdge(succ2);
					n1->ptrAnnot[0]=succ2.node_ptr;
					n2->ptrAnnot[0]=succ1.node_ptr;
					n1->ptrAnnot[1]=succ2.node_ptr;
					n2->ptrAnnot[1]=succ1.node_ptr;
				} 
			} else {
				n->intAnnot[polarity]=1;
				for (uint i=0;i<n->numEdges;i++) {
					Edge succ=n->edges[i];
					if(!edgeIsVarConst(succ)) {
						succ=constraintNegateIf(succ, polarity);
						pushVectorEdge(stack, succ);
					}
				}
			}
		}
	}
}

void convertPass(CNF *cnf, bool backtrackLit) {
	uint numConstraints=getSizeVectorEdge(&cnf->constraints);
	VectorEdge *ve=allocDefVectorEdge();
	for(uint i=0; i<numConstraints;i++) {
		convertConstraint(cnf, ve, getVectorEdge(&cnf->constraints, i), backtrackLit);
	}
	deleteVectorEdge(ve);
}

void convertConstraint(CNF *cnf, VectorEdge *stack, Edge root, bool backtrackLit) {
	Node *nroot=getNodePtrFromEdge(root);
	
	if (isNodeEdge(root) && (nroot->flags.type == NodeType_ITE || nroot->flags.type == NodeType_IFF)) {
		nroot = (Node *) nroot->ptrAnnot[isNegEdge(root)];
		root = (Edge) { nroot };
	}
	if (edgeIsConst(root)) {
		if (isNegEdge(root)) {
			//trivally unsat
			Edge newvar=constraintNewVar(cnf);
			Literal var=getEdgeVar(newvar);
			Literal clause[] = {var};
			addArrayClauseLiteral(cnf->solver, 1, clause);
			clause[0]=-var;
			addArrayClauseLiteral(cnf->solver, 1, clause);
			return;
		} else {
			//trivially true
			return;
		}
	} else if (edgeIsVarConst(root)) {
		Literal clause[] = { getEdgeVar(root)};
		addArrayClauseLiteral(cnf->solver, 1, clause);
		return;
	}
	
	clearVectorEdge(stack);pushVectorEdge(stack, root);
	while(getSizeVectorEdge(stack)!=0) {
		Edge e=lastVectorEdge(stack);
		Node *n=getNodePtrFromEdge(e);

		if (edgeIsVarConst(e)) {
			popVectorEdge(stack);
			continue;
		} else if (n->flags.type==NodeType_ITE ||
							 n->flags.type==NodeType_IFF) {
			popVectorEdge(stack);
			if (n->ptrAnnot[0]!=NULL)
				pushVectorEdge(stack, (Edge) {(Node *)n->ptrAnnot[0]});
			if (n->ptrAnnot[1]!=NULL)
				pushVectorEdge(stack, (Edge) {(Node *)n->ptrAnnot[1]});
			continue;
		}

		bool needPos = (n->intAnnot[0] > 0);
		bool needNeg = (n->intAnnot[1] > 0);
		if ((!needPos || n->flags.cnfVisitedUp & 1) &&
				(!needNeg || n->flags.cnfVisitedUp & 2)) {
			popVectorEdge(stack);
		} else if ((needPos && !(n->flags.cnfVisitedDown & 1)) ||
							 (needNeg && !(n->flags.cnfVisitedDown & 2))) {
			if (needPos)
				n->flags.cnfVisitedDown|=1;
			if (needNeg)
				n->flags.cnfVisitedDown|=2;
			for(uint i=0; i<n->numEdges; i++) {
				Edge arg=n->edges[i];
				arg=constraintNegateIf(arg, isNegEdge(e));
				pushVectorEdge(stack, arg); //WARNING, THIS LOOKS LIKE A BUG IN THE ORIGINAL CODE
			}
		} else {
			popVectorEdge(stack);
			produceCNF(cnf, e);
		}
	}
	CNFExpr * cnfExp = (CNFExpr *) nroot->ptrAnnot[isNegEdge(root)];
	ASSERT(cnfExp!=NULL);
	if (isProxy(cnfExp)) {
		Literal l=getProxy(cnfExp);
		Literal clause[] = {l};
		addArrayClauseLiteral(cnf->solver, 1, clause);
	} else if (backtrackLit) {
		Literal l=introProxy(cnf, root, cnfExp, isNegEdge(root));
		Literal clause[] = {l};
		addArrayClauseLiteral(cnf->solver, 1, clause);
	} else {
		outputCNF(cnf, cnfExp);
	}

	if (!((intptr_t) cnfExp & 1)) {
		deleteCNFExpr(cnfExp);
		nroot->ptrAnnot[isNegEdge(root)] = NULL;
	}
}


Literal introProxy(CNF * cnf, Edge e, CNFExpr* exp, bool isNeg) {
	Literal l = 0;
	Node * n = getNodePtrFromEdge(e);
	
	if (n->flags.cnfVisitedUp & (1<<!isNeg)) {
		CNFExpr* otherExp = (CNFExpr*) n->ptrAnnot[!isNeg];
		if (isProxy(otherExp))
			l = -getProxy(otherExp);
	} else {
		Edge semNeg={(Node *) n->ptrAnnot[isNeg]};
		Node * nsemNeg=getNodePtrFromEdge(semNeg);
		if (nsemNeg != NULL) {
			if (nsemNeg->flags.cnfVisitedUp & (1 << isNeg)) {
				CNFExpr* otherExp = (CNFExpr*) nsemNeg->ptrAnnot[isNeg];
				if (isProxy(otherExp))
					l = -getProxy(otherExp);
			} else if (nsemNeg->flags.cnfVisitedUp & (1<< !isNeg)) {
				CNFExpr* otherExp = (CNFExpr*) nsemNeg->ptrAnnot[!isNeg];
				if (isProxy(otherExp))
					l = getProxy(otherExp);
			}
		}
	}
	
	if (l == 0) {
		Edge newvar = constraintNewVar(cnf);
		l = getEdgeVar(newvar);
	}
	// Output the constraints on the auxiliary variable
	constrainCNF(cnf, l, exp);
	deleteCNFExpr(exp);
  
	n->ptrAnnot[isNeg] = (void*) ((intptr_t) (l << 1) | 1);
	
	return l;
}

void produceCNF(CNF * cnf, Edge e) {
	CNFExpr* expPos = NULL;
	CNFExpr* expNeg = NULL;
	Node *n = getNodePtrFromEdge(e);
	
	if (n->intAnnot[0] > 0) {
		expPos = produceConjunction(cnf, e);
	}

	if (n->intAnnot[1]  > 0) {
		expNeg = produceDisjunction(cnf, e);
	}

	/// @todo Propagate constants across semantic negations (this can
	/// be done similarly to the calls to propagate shown below).  The
	/// trick here is that we need to figure out how to get the
	/// semantic negation pointers, and ensure that they can have CNF
	/// produced for them at the right point
	///
	/// propagate(solver, expPos, snPos, false) || propagate(solver, expNeg, snNeg, false)
	
	// propagate from positive to negative, negative to positive
	if (!propagate(cnf, & expPos, expNeg, true))
		propagate(cnf, & expNeg, expPos, true);
	
	// The polarity heuristic entails visiting the discovery polarity first
	if (isPosEdge(e)) {
		saveCNF(cnf, expPos, e, false);
		saveCNF(cnf, expNeg, e, true);
	} else {
		saveCNF(cnf, expNeg, e, true);
		saveCNF(cnf, expPos, e, false);
	}
}

bool propagate(CNF *cnf, CNFExpr ** dest, CNFExpr * src, bool negate) {
	if (src != NULL && !isProxy(src) && getLitSizeCNF(src) == 0) {
		if (*dest == NULL) {
			*dest = allocCNFExprBool(negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
		} else if (isProxy(*dest)) {
			bool alwaysTrue = (negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
			if (alwaysTrue) {
				Literal clause[] = {getProxy(*dest)};
				addArrayClauseLiteral(cnf->solver, 1, clause);
			} else {
				Literal clause[] = {-getProxy(*dest)};
				addArrayClauseLiteral(cnf->solver, 1, clause);
			}
			
			*dest = allocCNFExprBool(negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
		} else {
			clearCNFExpr(*dest, negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
		}
		return true;
	}
	return false;
}

void saveCNF(CNF *cnf, CNFExpr* exp, Edge e, bool sign) {
	Node *n=getNodePtrFromEdge(e);
	n->flags.cnfVisitedUp |= (1 << sign);
	if (exp == NULL || isProxy(exp)) return;
  
	if (exp->litSize == 1) {
		Literal l = getLiteralLitVector(&exp->singletons, 0);
		deleteCNFExpr(exp);
		n->ptrAnnot[sign] = (void*) ((((intptr_t) l) << 1) | 1);
	} else if (exp->litSize != 0 && (n->intAnnot[sign] > 1 || n->flags.varForced)) {
		introProxy(cnf, e, exp, sign);
	} else {
		n->ptrAnnot[sign] = exp;
	}
}

void constrainCNF(CNF * cnf, Literal lcond, CNFExpr *expr) {
	if (alwaysTrueCNF(expr)) {
		return;
	} else if (alwaysFalseCNF(expr)) {
		Literal clause[] = {-lcond};
		addArrayClauseLiteral(cnf->solver, 1, clause);
		return;
	}
	
	for(uint i=0;i<getSizeLitVector(&expr->singletons);i++) {
		Literal l=getLiteralLitVector(&expr->singletons,i);
		Literal clause[] = {-lcond, l};
		addArrayClauseLiteral(cnf->solver, 2, clause);
	}
	for(uint i=0;i<getSizeVectorLitVector(&expr->clauses);i++) {
		LitVector *lv=getVectorLitVector(&expr->clauses,i);
		addClauseLiteral(cnf->solver, -lcond); //Add first literal
		addArrayClauseLiteral(cnf->solver, getSizeLitVector(lv), lv->literals); //Add rest
	}
}

void outputCNF(CNF *cnf, CNFExpr *expr) {
	for(uint i=0;i<getSizeLitVector(&expr->singletons);i++) {
		Literal l=getLiteralLitVector(&expr->singletons,i);
		Literal clause[] = {l};
		addArrayClauseLiteral(cnf->solver, 1, clause);
	}
	for(uint i=0;i<getSizeVectorLitVector(&expr->clauses);i++) {
		LitVector *lv=getVectorLitVector(&expr->clauses,i);
		addArrayClauseLiteral(cnf->solver, getSizeLitVector(lv), lv->literals);
	}
}

CNFExpr* fillArgs(CNF *cnf, Edge e, bool isNeg, Edge * largestEdge) {
	clearVectorEdge(&cnf->args);

	*largestEdge = (Edge) {(Node*) NULL};
	CNFExpr* largest = NULL;
	Node *n=getNodePtrFromEdge(e);
	int i = n->numEdges;
	while (i != 0) {
		Edge arg = n->edges[--i];
		arg=constraintNegateIf(arg, isNeg);
		Node * narg = getNodePtrFromEdge(arg);
		
		if (edgeIsVarConst(arg)) {
			pushVectorEdge(&cnf->args, arg);
			continue;
		}
		
		if (narg->flags.type == NodeType_ITE || narg->flags.type == NodeType_IFF) {
			arg = (Edge) {(Node *) narg->ptrAnnot[isNegEdge(arg)]};
		}
    
		if (narg->intAnnot[isNegEdge(arg)] == 1) {
			CNFExpr* argExp = (CNFExpr*) narg->ptrAnnot[isNegEdge(arg)];
			if (!isProxy(argExp)) {
				if (largest == NULL) {
					largest = argExp;
					* largestEdge = arg;
					continue;
				} else if (argExp->litSize > largest->litSize) {
					pushVectorEdge(&cnf->args, *largestEdge);
					largest = argExp;
					* largestEdge = arg;
					continue;
				}
			}
		}
		pushVectorEdge(&cnf->args, arg);
	}
	
	if (largest != NULL) {
		Node *nlargestEdge=getNodePtrFromEdge(*largestEdge);
		nlargestEdge->ptrAnnot[isNegEdge(*largestEdge)] = NULL;
	}
	
	return largest;
}

void printCNF(Edge e) {
	if (edgeIsVarConst(e)) {
		Literal l=getEdgeVar(e);
		model_print ("%d", l);
		return;
	}
	bool isNeg=isNegEdge(e);
	if (edgeIsConst(e)) {
		if (isNeg)
			model_print("T");
		else
			model_print("F");
		return;
	}
	Node *n=getNodePtrFromEdge(e);
	if (isNeg) {
		//Pretty print things that are equivalent to OR's
		if (getNodeType(e)==NodeType_AND) {
			model_print("or(");
			for(uint i=0;i<n->numEdges;i++) {
				Edge e=n->edges[i];
				if (i!=0)
					model_print(" ");
				printCNF(constraintNegate(e));
			}
			model_print(")");
			return;
		}

		model_print("!");
	}
	switch(getNodeType(e)) {
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
	for(uint i=0;i<n->numEdges;i++) {
		Edge e=n->edges[i];
		if (i!=0)
			model_print(" ");
		printCNF(e);
	}
	model_print(")");
}

CNFExpr * produceConjunction(CNF * cnf, Edge e) {
	Edge largestEdge;
	
	CNFExpr* accum = fillArgs(cnf, e, false, &largestEdge);
	if (accum == NULL)
		accum = allocCNFExprBool(true);
	
	int i = getSizeVectorEdge(&cnf->args);
	while (i != 0) {
		Edge arg = getVectorEdge(&cnf->args, --i);
		if (edgeIsVarConst(arg)) {
			conjoinCNFLit(accum, getEdgeVar(arg));
		} else {
			Node *narg=getNodePtrFromEdge(arg);
			CNFExpr* argExp = (CNFExpr*) narg->ptrAnnot[isNegEdge(arg)];
      
			bool destroy = (--narg->intAnnot[isNegEdge(arg)] == 0);
			if (isProxy(argExp)) { // variable has been introduced
				conjoinCNFLit(accum, getProxy(argExp));
			} else {
				conjoinCNFExpr(accum, argExp, destroy);
				if (destroy)
					narg->ptrAnnot[isNegEdge(arg)] = NULL;
			}
		}
	}
	
	return accum;
}

#define CLAUSE_MAX 3

CNFExpr* produceDisjunction(CNF *cnf, Edge e) {
	Edge largestEdge;
	CNFExpr* accum = fillArgs(cnf, e, true, &largestEdge);
	if (accum == NULL)
		accum = allocCNFExprBool(false);
	
	// This is necessary to check to make sure that we don't start out
	// with an accumulator that is "too large".
	
	/// @todo Strictly speaking, introProxy doesn't *need* to free
	/// memory, then this wouldn't have to reallocate CNFExpr
	
	/// @todo When this call to introProxy is made, the semantic
	/// negation pointer will have been destroyed.  Thus, it will not
	/// be possible to use the correct proxy.  That should be fixed.
	
	// at this point, we will either have NULL, or a destructible expression
	if (getClauseSizeCNF(accum) > CLAUSE_MAX)
		accum = allocCNFExprLiteral(introProxy(cnf, largestEdge, accum, isNegEdge(largestEdge)));
	
	int i = getSizeVectorEdge(&cnf->args);
	while (i != 0) {
		Edge arg=getVectorEdge(&cnf->args, --i);
		Node *narg=getNodePtrFromEdge(arg);
		if (edgeIsVarConst(arg)) {
			disjoinCNFLit(accum, getEdgeVar(arg));
		} else {
			CNFExpr* argExp = (CNFExpr*) narg->ptrAnnot[isNegEdge(arg)];
			
			bool destroy = (--narg->intAnnot[isNegEdge(arg)] == 0);
			if (isProxy(argExp)) { // variable has been introduced
				disjoinCNFLit(accum, getProxy(argExp));
			} else if (argExp->litSize == 0) {
				disjoinCNFExpr(accum, argExp, destroy);
			} else {
				// check to see if we should introduce a proxy
				int aL = accum->litSize;      // lits in accum
				int eL = argExp->litSize;     // lits in argument
				int aC = getClauseSizeCNF(accum);   // clauses in accum
				int eC = getClauseSizeCNF(argExp);  // clauses in argument
				
				if (eC > CLAUSE_MAX || (eL * aC + aL * eC > eL + aC + aL + aC)) {
					disjoinCNFLit(accum, introProxy(cnf, arg, argExp, isNegEdge(arg)));
				} else {
					disjoinCNFExpr(accum, argExp, destroy);
					if (destroy) narg->ptrAnnot[isNegEdge(arg)] = NULL;
				}
			}
		}
	}
  
	return accum;
}

Edge generateBinaryConstraint(CNF *cnf, uint numvars, Edge * vars, uint value) {
	Edge carray[numvars];
	for(uint j=0;j<numvars;j++) {
		carray[j]=((value&1)==1) ? vars[j] : constraintNegate(vars[j]);
		value=value>>1;
	}
	
	return constraintAND(cnf, numvars, carray);
}
 
/** Generates a constraint to ensure that all encodings are less than value */
Edge generateLTConstraint(CNF *cnf, uint numvars, Edge * vars, uint value) {
	Edge orarray[numvars];
	Edge andarray[numvars];
	uint andi=0;
  
	while(true) {
		uint val=value;
		uint ori=0;
		for(uint j=0;j<numvars;j++) {
			if ((val&1)==1)
				orarray[ori++]=constraintNegate(vars[j]);
			val=val>>1;
		}
		//no ones to flip, so bail now...
		if (ori==0) {
			return constraintAND(cnf, andi, andarray);
		}
		andarray[andi++]=constraintOR(cnf, ori, orarray);
		
		value=value+(1<<(__builtin_ctz(value)));
		//flip the last one
	}
}
  
Edge generateEquivNVConstraint(CNF *cnf, uint numvars, Edge *var1, Edge *var2) {
	if (numvars==0)
		return E_True;
	Edge array[numvars];
	for(uint i=0;i<numvars;i++) {
		array[i]=constraintIFF(cnf, var1[i], var2[i]);
	}
	return constraintAND(cnf, numvars, array);
}
