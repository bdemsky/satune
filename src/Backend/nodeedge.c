#include "nodeedge.h"
#include <string.h>
#include <stdlib.h>

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
