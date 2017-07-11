/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#include "constraint.h"
#include "mymemory.h"
#include "inc_solver.h"

Constraint ctrue={TRUE, 0xffffffff, NULL, NULL};
Constraint cfalse={FALSE, 0xffffffff, NULL, NULL};

Constraint * allocConstraint(CType t, Constraint *l, Constraint *r) {
	Constraint *This=(Constraint *) ourmalloc(sizeof(Constraint));
	This->type=t;
	This->numoperandsorvar=2;
	This->operands=(Constraint **)ourmalloc(2*sizeof(Constraint *));
	This->neg=NULL;
	ASSERT(l!=NULL);
	//if (type==IMPLIES) {
	//type=OR;
	//	operands[0]=l->negate();
	//	} else {
	This->operands[0]=l;
	//	}
	This->operands[1]=r;
	return This;
}

Constraint * allocUnaryConstraint(CType t, Constraint *l) {
	Constraint *This=(Constraint *) ourmalloc(sizeof(Constraint));
	This->type=t;
	This->numoperandsorvar=1;
	This->operands=(Constraint **) ourmalloc(sizeof(Constraint *));
	This->neg=NULL;
	This->operands[0]=l;
	return This;
}

Constraint * allocArrayConstraint(CType t, uint num, Constraint **array) {
	Constraint *This=(Constraint *) ourmalloc(sizeof(Constraint));
	This->type=t;
	This->numoperandsorvar=num;
	This->operands=(Constraint **) ourmalloc(num*sizeof(Constraint *));
	This->neg=NULL;
	memcpy(This->operands, array, num*sizeof(Constraint *));
	return This;
}

Constraint * allocVarConstraint(CType t, uint v) {
	Constraint *This=(Constraint *) ourmalloc(sizeof(Constraint));
	This->type=t;
	This->numoperandsorvar=v;
	This->operands=NULL;
	This->neg=NULL;
	return This;
}

void deleteConstraint(Constraint *This) {
	if (This->operands!=NULL)
		ourfree(This->operands);
	ourfree(This);
}

void dumpConstraint(Constraint * This, IncrementalSolver *solver) {
	if (This->type==VAR) {
		addClauseLiteral(solver, This->numoperandsorvar);
		addClauseLiteral(solver, 0);
	} else if (This->type==NOTVAR) {
		addClauseLiteral(solver, -This->numoperandsorvar);
		addClauseLiteral(solver, 0);
	} else {
		ASSERT(This->type==OR);
		for(uint i=0;i<This->numoperandsorvar;i++) {
			Constraint *c=This->operands[i];
			if (c->type==VAR) {
				addClauseLiteral(solver, c->numoperandsorvar);
			} else if (c->type==NOTVAR) {
				addClauseLiteral(solver, -c->numoperandsorvar);
			} else ASSERT(0);
		}
		addClauseLiteral(solver, 0);
	}
}

void internalfreeConstraint(Constraint * This) {
	switch(This->type) {
	case TRUE:
	case FALSE:
	case NOTVAR:
	case VAR:
		return;
	case BOGUS:
		ASSERT(0);
	default:
		This->type=BOGUS;
		deleteConstraint(This);
	}
}

void freerecConstraint(Constraint *This) {
	switch(This->type) {
	case TRUE:
	case FALSE:
	case NOTVAR:
	case VAR:
		return;
	case BOGUS:
		ASSERT(0);
	default:
		if (This->operands!=NULL) {
			for(uint i=0;i<This->numoperandsorvar;i++)
				freerecConstraint(This->operands[i]);
		}
		This->type=BOGUS;
		deleteConstraint(This);
	}
}


void printConstraint(Constraint * This) {
	switch(This->type) {
	case TRUE:
		model_print("true");
		break;
	case FALSE:
		model_print("false");
		break;
	case IMPLIES:
		model_print("(");
		printConstraint(This->operands[0]);
		model_print(")");
		model_print("=>");
		model_print("(");
		printConstraint(This->operands[1]);
		model_print(")");
		break;
	case AND:
	case OR:
		model_print("(");
		for(uint i=0;i<This->numoperandsorvar;i++) {
			if (i!=0) {
				if (This->type==AND)
					model_print(" ^ ");
				else
					model_print(" v ");
			}
			printConstraint(This->operands[i]);
		}
		model_print(")");
		break;
	case VAR:
		model_print("t%u",This->numoperandsorvar);
		break;
	case NOTVAR:
		model_print("!t%u",This->numoperandsorvar);
		break;
	default:
		model_print("In printingConstraint: %d", This->type);
		ASSERT(0);
	}
}

Constraint * cloneConstraint(Constraint * This) {
	switch(This->type) {
	case TRUE:
	case FALSE:
	case VAR:
	case NOTVAR:
		return This;
	case IMPLIES:
		return allocConstraint(IMPLIES, cloneConstraint(This->operands[0]), cloneConstraint(This->operands[1]));
	case AND:
	case OR: {
		Constraint *array[This->numoperandsorvar];
		for(uint i=0;i<This->numoperandsorvar;i++) {
			array[i]=cloneConstraint(This->operands[i]);
		}
		return allocArrayConstraint(This->type, This->numoperandsorvar, array);
	}
	default:
		ASSERT(0);
		return NULL;
	}
}

Constraint * generateBinaryConstraint(uint numvars, Constraint ** vars, uint value) {
	Constraint *carray[numvars];
	for(uint j=0;j<numvars;j++) {
		carray[j]=((value&1)==1) ? vars[j] : negateConstraint(vars[j]);
		value=value>>1;
	}

	return allocArrayConstraint(AND, numvars, carray);
}

/** Generates a constraint to ensure that all encodings are less than value */
Constraint * generateLTConstraint(uint numvars, Constraint ** vars, uint value) {
	Constraint *orarray[numvars];
	Constraint *andarray[numvars];
	uint andi=0;

	while(true) {
		uint val=value;
		uint ori=0;
		for(uint j=0;j<numvars;j++) {
			if ((val&1)==1)
				orarray[ori++]=negateConstraint(vars[j]);
			val=val>>1;
		}
		//no ones to flip, so bail now...
		if (ori==0) {
			return allocArrayConstraint(AND, andi, andarray);
		}
		andarray[andi++]=allocArrayConstraint(OR, ori, orarray);

		value=value+(1<<(__builtin_ctz(value)));
		//flip the last one
	}
}

Constraint * generateEquivNVConstraint(uint numvars, Constraint **var1, Constraint **var2) {
	if (numvars==0)
		return &ctrue;
	Constraint *array[numvars*2];
	for(uint i=0;i<numvars;i++) {
		array[i*2]=allocConstraint(OR, negateConstraint(cloneConstraint(var1[i])), var2[i]);
		array[i*2+1]=allocConstraint(OR, var1[i], negateConstraint(cloneConstraint(var2[i])));
	}
	return allocArrayConstraint(AND, numvars*2, array);
}

Constraint * generateEquivConstraint(Constraint *var1, Constraint *var2) {
	Constraint * imp1=allocConstraint(OR, negateConstraint(cloneConstraint(var1)), var2);
	Constraint * imp2=allocConstraint(OR, var1, negateConstraint(cloneConstraint(var2)));

	return allocConstraint(AND, imp1, imp2);
}

bool mergeandfree(VectorConstraint * to, VectorConstraint * from) {
	for(uint i=0;i<getSizeVectorConstraint(from);i++) {
		Constraint *c=getVectorConstraint(from, i);
		if (c->type==TRUE)
			continue;
		if (c->type==FALSE) {
			for(uint j=i+1;j<getSizeVectorConstraint(from);j++)
				freerecConstraint(getVectorConstraint(from,j));
			for(uint j=0;j<getSizeVectorConstraint(to);j++)
				freerecConstraint(getVectorConstraint(to, j));
			clearVectorConstraint(to);
			pushVectorConstraint(to, &ctrue);
			deleteVectorConstraint(from);
			return true;
		}
		pushVectorConstraint(to, c);
	}
	deleteVectorConstraint(from);
	return false;
}

VectorConstraint * simplifyConstraint(Constraint * This) {
	switch(This->type) {
	case TRUE:
	case VAR:
	case NOTVAR:
	case FALSE: {
		VectorConstraint * vec=allocDefVectorConstraint();
		pushVectorConstraint(vec, This);
		return vec;
	}
	case AND: {
		VectorConstraint *vec=allocDefVectorConstraint();
		for(uint i=0;i<This->numoperandsorvar;i++) {
			VectorConstraint * subvec=simplifyConstraint(This->operands[i]);
			if (mergeandfree(vec, subvec)) {
				for(uint j=i+1;j<This->numoperandsorvar;j++) {
					freerecConstraint(This->operands[j]);
				}
				internalfreeConstraint(This);
				return vec;
			}
		}
		internalfreeConstraint(This);
		return vec;
	}
	case OR: {
		for(uint i=0;i<This->numoperandsorvar;i++) {
			Constraint *c=This->operands[i];
			switch(c->type) {
			case TRUE: {
				VectorConstraint * vec=allocDefVectorConstraint();
				pushVectorConstraint(vec, c);
				freerecConstraint(This);
				return vec;
			}
			case FALSE: {
				Constraint *array[This->numoperandsorvar-1];
				uint index=0;
				for(uint j=0;j<This->numoperandsorvar;j++) {
					if (j!=i)
						array[index++]=This->operands[j];
				}
				Constraint *cn=allocArrayConstraint(OR, index, array);
				VectorConstraint *vec=simplifyConstraint(cn);
				internalfreeConstraint(This);
				return vec;
			}
			case VAR:
			case NOTVAR:
				break;
			case OR: {
				uint nsize=This->numoperandsorvar+c->numoperandsorvar-1;
				Constraint *array[nsize];
				uint index=0;
				for(uint j=0;j<This->numoperandsorvar;j++)
					if (j!=i)
						array[index++]=This->operands[j];
				for(uint j=0;j<c->numoperandsorvar;j++)
					array[index++]=c->operands[j];
				Constraint *cn=allocArrayConstraint(OR, nsize, array);
				VectorConstraint *vec=simplifyConstraint(cn);
				internalfreeConstraint(This);
				internalfreeConstraint(c);
				return vec;
			}
			case IMPLIES: {
				uint nsize=This->numoperandsorvar+1;
				Constraint *array[nsize];
				uint index=0;
				for(uint j=0;j<This->numoperandsorvar;j++)
					if (j!=i)
						array[index++]=This->operands[j];
				array[index++]=negateConstraint(c->operands[0]);
				array[index++]=c->operands[1];
				Constraint *cn=allocArrayConstraint(OR, nsize, array);
				VectorConstraint *vec=simplifyConstraint(cn);
				internalfreeConstraint(This);
				internalfreeConstraint(c);
				return vec;
			}
			case AND: {
				Constraint *array[This->numoperandsorvar];

				VectorConstraint *vec=allocDefVectorConstraint();
				for(uint j=0;j<c->numoperandsorvar;j++) {
					//copy other elements
					for(uint k=0;k<This->numoperandsorvar;k++) {
						if (k!=i) {
							array[k]=cloneConstraint(This->operands[k]);
						}
					}

					array[i]=cloneConstraint(c->operands[j]);
					Constraint *cn=allocArrayConstraint(OR, This->numoperandsorvar, array);
					VectorConstraint * newvec=simplifyConstraint(cn);
					if (mergeandfree(vec, newvec)) {
						freerecConstraint(This);
						return vec;
					}
				}
				freerecConstraint(This);
				return vec;
			}
			default:
				ASSERT(0);
			}
			//continue on to next item
		}
		VectorConstraint * vec=allocDefVectorConstraint();
		if (This->numoperandsorvar==1) {
			Constraint *c=This->operands[0];
			freerecConstraint(This);
			pushVectorConstraint(vec, c);
		} else
			pushVectorConstraint(vec, This);
		return vec;
	}
	case IMPLIES: {
		Constraint *cn=allocConstraint(OR, negateConstraint(This->operands[0]), This->operands[1]);
		VectorConstraint * vec=simplifyConstraint(cn);
		internalfreeConstraint(This);
		return vec;
	}
	default:
		ASSERT(0);
		return NULL;
	}
}

Constraint * negateConstraint(Constraint * This) {
	switch(This->type) {
	case TRUE:
		return &cfalse;
	case FALSE:
		return &ctrue;
	case NOTVAR:
	case VAR:
		return This->neg;
	case IMPLIES: {
		Constraint *l=This->operands[0];
		Constraint *r=This->operands[1];
		This->operands[0]=r;
		This->operands[1]=l;
		return This;
	}
	case AND:
	case OR: {
		for(uint i=0;i<This->numoperandsorvar;i++) {
			This->operands[i]=negateConstraint(This->operands[i]);
		}
		This->type=(This->type==AND) ? OR : AND;
		return This;
	}
	default:
		ASSERT(0);
		return NULL;
	}
}
