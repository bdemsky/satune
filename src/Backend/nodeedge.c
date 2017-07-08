#include "nodeedge.h"
#include <string.h>
#include <stdlib.h>
#include "inc_solver.h"

/** Code ported from C++ BAT implementation of NICE-SAT */

VectorImpl(Edge, Edge, 16)

CNF * createCNF() {
	CNF * cnf=ourmalloc(sizeof(CNF));
	cnf->varcount=1;
	cnf->capacity=DEFAULT_CNF_ARRAY_SIZE;
	cnf->mask=cnf->capacity-1;
	cnf->node_array=ourcalloc(1, sizeof(Node *)*cnf->capacity);
	cnf->size=0;
	cnf->maxsize=(uint)(((double)cnf->capacity)*LOAD_FACTOR);
	cnf->enableMatching=true;
	allocInlineDefVectorEdge(& cnf->constraints);
 return cnf;
}

void deleteCNF(CNF * cnf) {
	for(uint i=0;i<cnf->capacity;i++) {
		Node *n=cnf->node_array[i];
		if (n!=NULL)
			ourfree(n);
	}
	deleteVectorArrayEdge(& cnf->constraints);
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
		hashvalue ^= (uint) edges[i].node_ptr;
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
	Edge eand=constraintAND2(cnf, left, right);
	return constraintNegate(eand);
}

int comparefunction(const Edge * e1, const Edge * e2) {
	return ((uintptr_t)e1->node_ptr)-((uintptr_t)e2->node_ptr);
}

Edge constraintAND(CNF * cnf, uint numEdges, Edge * edges) {
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
	edges[lowindex++]=edges[initindex++];

	for(;initindex<numEdges;initindex++) {
		Edge e1=edges[lowindex];
		Edge e2=edges[initindex];
		if (sameNodeVarEdge(e1, e2)) {
			if (!sameSignEdge(e1, e2)) {
				return E_False;
			}
		} else
			edges[lowindex++]=edges[initindex];
	}
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
	
	return createNode(cnf, NodeType_AND, numEdges, edges);
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
	bool negate=sameSignEdge(left, right);
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

void addConstraint(CNF *cnf, Edge constraint) {
	pushVectorEdge(&cnf->constraints, constraint);
}

Edge constraintNewVar(CNF *cnf) {
	uint varnum=cnf->varcount++;
	Edge e={(Node *) ((((uintptr_t)varnum) << VAR_SHIFT) | EDGE_IS_VAR_CONSTANT) };
	return e;
}

void countPass(CNF *cnf) {
	uint numConstraints=getSizeVectorEdge(&cnf->constraints);
	VectorEdge *ve=allocDefVectorEdge();
	for(uint i=0; i<numConstraints;i++) {
		countConstraint(cnf, ve, getVectorEdge(&cnf->constraints, i));
	}
	deleteVectorEdge(ve);
}

void countConstraint(CNF *cnf, VectorEdge *stack, Edge e) {
	//Skip constants and variables...
	if (edgeIsVarConst(e))
		return;

	clearVectorEdge(stack);pushVectorEdge(stack, e);

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
			setExpanded(n, polarity); n->intAnnot[polarity]=1;
			
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
				for (uint i=0;i<n->numEdges;i++) {
					Edge succ=n->edges[i];
					succ=constraintNegateIf(succ, polarity);
					if(!edgeIsVarConst(succ)) {
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
		root = (Edge) { (Node *) nroot->ptrAnnot[isNegEdge(root)]};
	}
	
	if (edgeIsConst(root)) {
		if (isNegEdge(root)) {
			//trivally unsat
			Edge newvar=constraintNewVar(cnf);
			Literal var=getEdgeVar(newvar);
			Literal clause[] = {var, -var, 0};
			addArrayClauseLiteral(cnf->solver, 3, clause);
			return;
		} else {
			//trivially true
			return;
		}
	} else if (edgeIsVarConst(root)) {
		Literal clause[] = { getEdgeVar(root), 0};
		addArrayClauseLiteral(cnf->solver, 2, clause);
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
			pushVectorEdge(stack, (Edge) {(Node *)n->ptrAnnot[0]});
			pushVectorEdge(stack, (Edge) {(Node *)n->ptrAnnot[1]});
			continue;
		}

		bool needPos = (n->intAnnot[0] > 0);
		bool needNeg = (n->intAnnot[1] > 0);
		if ((!needPos || n->flags.cnfVisitedUp & 1) ||
				(!needNeg || n->flags.cnfVisitedUp & 2)) {
			popVectorEdge(stack);
		} else if ((needPos && !n->flags.cnfVisitedDown & 1) ||
							 (needNeg && !n->flags.cnfVisitedDown & 2)) {
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
	if (isProxy(cnfExp)) {
		//solver.add(getProxy(cnfExp))
	} else if (backtrackLit) {
		//solver.add(introProxy(solver, root, cnfExp, isNegEdge(root)));
	} else {
		//solver.add(*cnfExp);
	}

	if (!((intptr_t) cnfExp & 1)) {
		//free rootExp
		nroot->ptrAnnot[isNegEdge(root)] = NULL;
	}
}

//DONE
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
	//delete exp; //FIXME
  
	n->ptrAnnot[isNeg] = (void*) ((intptr_t) (l << 1) | 1);
	
	return l;
}

//DONE
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
	propagate(cnf, expPos, expNeg, true) || propagate(cnf, expNeg, expPos, true);
	
	// The polarity heuristic entails visiting the discovery polarity first
	if (isPosEdge(e)) {
		saveCNF(cnf, expPos, e, false);
		saveCNF(cnf, expNeg, e, true);
	} else {
		saveCNF(cnf, expNeg, e, true);
		saveCNF(cnf, expPos, e, false);
	}
}


//DONE
bool propagate(CNF *cnf, CNFExpr * dest, CNFExpr * src, bool negate) {
	if (src != NULL && !isProxy(src) && getLitSizeCNF(src) == 0) {
		if (dest == NULL) {
			dest = allocCNFExprBool(negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
		} else if (isProxy(dest)) {
			bool alwaysTrue = (negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
			if (alwaysTrue) {
				Literal clause[] = {getProxy(dest), 0};
				addArrayClauseLiteral(cnf->solver, 2, clause);
			} else {
				Literal clause[] = {-getProxy(dest), 0};
				addArrayClauseLiteral(cnf->solver, 2, clause);
			}
			
			dest = allocCNFExprBool(negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
		} else {
			clearCNF(dest, negate ? alwaysFalseCNF(src) : alwaysTrueCNF(src));
		}
		return true;
	}
	return false;
}

void saveCNF(CNF *cnf, CNFExpr* exp, Edge e, bool sign) {
	Node *n=getNodePtrFromEdge(e);
	n->flags.cnfVisitedUp & (1 << sign);
	if (exp == NULL || isProxy(exp)) return;
  
	if (exp->litSize == 1) {
		Literal l = exp->singletons()[0];
		delete exp;
		n->ptrAnnot[sign] = (void*) ((intptr_t) (l << 1) | 1);
	} else if (exp->litSize != 0 && (n->intAnnot[sign] > 1 || e->isVarForced())) {
		introProxy(solver, e, exp, sign);
	} else {
		n->ptrAnnot[sign] = exp;
	}
}

void constrainCNF(CNF * cnf, Literal l, CNFExpr *exp) {
	if (alwaysTrueCNF(exp)) {
		return;
	} else if (alwaysFalseCNF(expr)) {
		Literal clause[] = {-l, 0};
		addArrayClauseLiteral(cnf->solver, 2, clause);
		return;
	}
	//FIXME
	
}

void outputCNF(CNF *cnf, CNFExpr *exp) {
	
}

CNFExpr* fillArgs(CNF *cnf, Edge e, bool isNeg, Edge * largestEdge, VectorEdge * args) {
	args.clear();

	*largestEdge = (void*) NULL;
	CnfExp* largest = NULL;
	int i = e->size();
	while (i != 0) {
		Edge arg = (*e)[--i]; arg.negateIf(isNeg);
		
		if (arg.isVar()) {
			args.push(arg);
			continue;
		}
		
		if (arg->op() == NodeOp_Ite || arg->op() == NodeOp_Iff) {
			arg = arg->ptrAnnot(arg.isNeg());
		}
    
		if (arg->intAnnot(arg.isNeg()) == 1) {
			CnfExp* argExp = (CnfExp*) arg->ptrAnnot(arg.isNeg());
			if (!isProxy(argExp)) {
				if (largest == NULL) {
					largest = argExp;
					largestEdge = arg;
					continue;
				} else if (argExp->litSize > largest->litSize) {
					args.push(largestEdge);
					largest = argExp;
					largestEdge = arg;
					continue;
				}
			}
		}
		args.push(arg);
	}
	
	if (largest != NULL) {
		largestEdge->ptrAnnot(largestEdge.isNeg()) = NULL;
	}
	
	return largest;
}


CNFExpr * produceConjunction(CNF * cnf, Edge e) {
	Edge largestEdge;
	CnfExp* accum = fillArgs(e, false, largestEdge);
	if (accum == NULL) accum = new CnfExp(true);
	
	int i = _args.size();
	while (i != 0) {
		Edge arg(_args[--i]);
		if (arg.isVar()) {
			accum->conjoin(atomLit(arg));
		} else {
			CnfExp* argExp = (CnfExp*) arg->ptrAnnot(arg.isNeg());
      
			bool destroy = (--arg->intAnnot(arg.isNeg()) == 0);
			if (isProxy(argExp)) { // variable has been introduced
				accum->conjoin(getProxy(argExp));
			} else {
				accum->conjoin(argExp, destroy);
				if (destroy) arg->ptrAnnot(arg.isNeg()) = NULL;
			}
		}
	}
	
	return accum;
}

#define CLAUSE_MAX 3

CNFExpr* produceDisjunction(CNF *cnf, Edge e) {
	Edge largestEdge;
	CNFExpr* accum = fillArgs(e, true, largestEdge);
	if (accum == NULL)
		accum = new CNFExpr(false);
	
	// This is necessary to check to make sure that we don't start out
	// with an accumulator that is "too large".
	
	/// @todo Strictly speaking, introProxy doesn't *need* to free
	/// memory, then this wouldn't have to reallocate CNFExpr
	
	/// @todo When this call to introProxy is made, the semantic
	/// negation pointer will have been destroyed.  Thus, it will not
	/// be possible to use the correct proxy.  That should be fixed.
	
	// at this point, we will either have NULL, or a destructible expression
	if (accum->clauseSize() > CLAUSE_MAX)
		accum = new CNFExpr(introProxy(solver, largestEdge, accum, largestEdge.isNeg()));
	
	int i = _args.size();
	while (i != 0) {
		Edge arg(_args[--i]);
		if (arg.isVar()) {
			accum->disjoin(atomLit(arg));
		} else {
			CNFExpr* argExp = (CNFExpr*) arg->ptrAnnot(arg.isNeg());
			
			bool destroy = (--arg->intAnnot(arg.isNeg()) == 0);
			if (isProxy(argExp)) { // variable has been introduced
				accum->disjoin(getProxy(argExp));
			} else if (argExp->litSize == 0) {
				accum->disjoin(argExp, destroy);
			} else {
				// check to see if we should introduce a proxy
				int aL = accum->litSize;      // lits in accum
				int eL = argExp->litSize;     // lits in argument
				int aC = getClauseSizeCNF(accum);   // clauses in accum
				int eC = getClauseSizeCNF(argExp);  // clauses in argument
				
				if (eC > CLAUSE_MAX || (eL * aC + aL * eC > eL + aC + aL + aC)) {
					accum->disjoin(introProxy(solver, arg, argExp, arg.isNeg()));
				} else {
					accum->disjoin(argExp, destroy);
					if (destroy) arg->ptrAnnot(arg.isNeg()) = NULL;
				}
			}
		}
	}
  
	return accum;
}
