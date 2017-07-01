#include "nodeedge.h"
#include <string.h>

CNF * createCNF() {
	CNF * cnf=ourmalloc(sizeof(CNF));
	cnf->varcount=1;
	cnf->capacity=DEFAULT_CNF_ARRAY_SIZE;
	cnf->mask=cnf->capacity-1;
	cnf->node_array=ourcalloc(1, sizeof(Node *)*cnf->capacity);
	cnf->size=0;
	cnf->maxsize=(uint)(((double)cnf->capacity)*LOAD_FACTOR);
	return cnf;
}

void deleteCNF(CNF * cnf) {
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
	n->numEdges=numEdges;
	n->flags.type=type;
	n->hashCode=hashcode;
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
		if (nodeedges[i].node_ptr!=edges[i].node_ptr)
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

Edge constraintAND(CNF * cnf, uint numEdges, Edge * edges) {
	return createNode(cnf, NodeType_AND, numEdges, edges);
}

Edge constraintAND2(CNF * cnf, Edge left, Edge right) {
	Edge edges[2]={left, right};
	return createNode(cnf, NodeType_AND, 2, edges);
}

Edge constraintIMPLIES(CNF * cnf, Edge left, Edge right) {
	Edge array[2];
	array[0]=left;
	array[1]=constraintNegate(right);
	Edge eand=constraintAND(cnf, 2, array);
	return constraintNegate(eand);
}

Edge constraintIFF(CNF * cnf, Edge left, Edge right) {
	Edge edges[]={left, right};
	return createNode(cnf, NodeType_IFF, 2, edges);
}

Edge constraintITE(CNF * cnf, Edge cond, Edge thenedge, Edge elseedge) {
	Edge edges[]={cond, thenedge, elseedge};
	return createNode(cnf, NodeType_ITE, 3, edges);
}

Edge constraintNewVar(CNF *cnf) {
	uint varnum=cnf->varcount++;
	Edge e={(Node *) ((((uintptr_t)varnum) << VAR_SHIFT) | EDGE_IS_VAR_CONSTANT) };
	return e;
}
