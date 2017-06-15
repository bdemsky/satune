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
	Constraint *this=(Constraint *) ourmalloc(sizeof(Constraint));
	this->type=t;
	this->numoperandsorvar=2;
	this->operands=(Constraint **)ourmalloc(2*sizeof(Constraint *));
	this->neg=NULL;
	ASSERT(l!=NULL);
	//if (type==IMPLIES) {
	//type=OR;
	//	operands[0]=l->negate();
	//	} else {
	this->operands[0]=l;
	//	}
	this->operands[1]=r;
	return this;
}

Constraint * allocUnaryConstraint(CType t, Constraint *l) {
	Constraint *this=(Constraint *) ourmalloc(sizeof(Constraint));
	this->type=t;
	this->numoperandsorvar=1;
	this->operands=(Constraint **) ourmalloc(sizeof(Constraint *));
	this->neg=NULL;
	this->operands[0]=l;
	return this;
}

Constraint * allocArrayConstraint(CType t, uint num, Constraint **array) {
	Constraint *this=(Constraint *) ourmalloc(sizeof(Constraint));
	this->type=t;
	this->numoperandsorvar=num;
	this->operands=(Constraint **) ourmalloc(num*sizeof(Constraint *));
	this->neg=NULL;
	memcpy(this->operands, array, num*sizeof(Constraint *));
	return this;
}

Constraint * allocVarConstraint(CType t, uint v) {
	Constraint *this=(Constraint *) ourmalloc(sizeof(Constraint));
	this->type=t;
	this->numoperandsorvar=v;
	this->operands=NULL;
	this->neg=NULL;
	return this;
}

void deleteConstraint(Constraint *this) {
	if (this->operands!=NULL)
		ourfree(this->operands);
}

void dumpConstraint(Constraint * this, IncrementalSolver *solver) {
	if (this->type==VAR) {
		addClauseLiteral(solver, this->numoperandsorvar);
		addClauseLiteral(solver, 0);
	} else if (this->type==NOTVAR) {
		addClauseLiteral(solver, -this->numoperandsorvar);
		addClauseLiteral(solver, 0);
	} else {
		ASSERT(this->type==OR);
		for(uint i=0;i<this->numoperandsorvar;i++) {
			Constraint *c=this->operands[i];
			if (c->type==VAR) {
				addClauseLiteral(solver, c->numoperandsorvar);
			} else if (c->type==NOTVAR) {
				addClauseLiteral(solver, -c->numoperandsorvar);
			} else ASSERT(0);
		}
		addClauseLiteral(solver, 0);
	}
}

void internalfreeConstraint(Constraint * this) {
	switch(this->type) {
	case TRUE:
	case FALSE:
	case NOTVAR:
	case VAR:
		return;
	case BOGUS:
		ASSERT(0);
	default:
		this->type=BOGUS;
		ourfree(this);
	}
}

void freerecConstraint(Constraint *this) {
	switch(this->type) {
	case TRUE:
	case FALSE:
	case NOTVAR:
	case VAR:
		return;
	case BOGUS:
		ASSERT(0);
	default:
		if (this->operands!=NULL) {
			for(uint i=0;i<this->numoperandsorvar;i++)
				freerecConstraint(this->operands[i]);
		}
		this->type=BOGUS;
		deleteConstraint(this);
	}
}


void printConstraint(Constraint * this) {
	switch(this->type) {
	case TRUE:
		model_print("true");
		break;
	case FALSE:
		model_print("false");
		break;
	case IMPLIES:
		model_print("(");
		printConstraint(this->operands[0]);
		model_print(")");
		model_print("=>");
		model_print("(");
		printConstraint(this->operands[1]);
		model_print(")");
		break;
	case AND:
	case OR:
		model_print("(");
		for(uint i=0;i<this->numoperandsorvar;i++) {
			if (i!=0) {
				if (this->type==AND)
					model_print(" ^ ");
				else
					model_print(" v ");
			}
			printConstraint(this->operands[i]);
		}
		model_print(")");
		break;
	case VAR:
		model_print("t%u",this->numoperandsorvar);
		break;
	case NOTVAR:
		model_print("!t%u",this->numoperandsorvar);
		break;
	default:
		ASSERT(0);
	}
}

Constraint * cloneConstraint(Constraint * this) {
	switch(this->type) {
	case TRUE:
	case FALSE:
	case VAR:
	case NOTVAR:
		return this;
	case IMPLIES:
		return allocConstraint(IMPLIES, cloneConstraint(this->operands[0]), cloneConstraint(this->operands[1]));
	case AND:
	case OR: {
		Constraint *array[this->numoperandsorvar];
		for(uint i=0;i<this->numoperandsorvar;i++) {
			array[i]=cloneConstraint(this->operands[i]);
		}
		return allocArrayConstraint(this->type, this->numoperandsorvar, array);
	}
	default:
		ASSERT(0);
		return NULL;
	}
}

Constraint * generateConstraint(uint numvars, Constraint ** vars, uint value) {
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

VectorConstraint * simplifyConstraint(Constraint * this) {
	switch(this->type) {
	case TRUE:
	case VAR:
	case NOTVAR:
	case FALSE: {
		VectorConstraint * vec=allocDefVectorConstraint();
		pushVectorConstraint(vec, this);
		return vec;
	}
	case AND: {
		VectorConstraint *vec=allocDefVectorConstraint();
		for(uint i=0;i<this->numoperandsorvar;i++) {
			VectorConstraint * subvec=simplifyConstraint(this->operands[i]);
			if (mergeandfree(vec, subvec)) {
				for(uint j=i+1;j<this->numoperandsorvar;j++) {
					freerecConstraint(this->operands[j]);
				}
				internalfreeConstraint(this);
				return vec;
			}
		}
		internalfreeConstraint(this);
		return vec;
	}
	case OR: {
		for(uint i=0;i<this->numoperandsorvar;i++) {
			Constraint *c=this->operands[i];
			switch(c->type) {
			case TRUE: {
				VectorConstraint * vec=allocDefVectorConstraint();
				pushVectorConstraint(vec, c);
				freerecConstraint(this);
				return vec;
			}
			case FALSE: {
				Constraint *array[this->numoperandsorvar-1];
				uint index=0;
				for(uint j=0;j<this->numoperandsorvar;j++) {
					if (j!=i)
						array[index++]=this->operands[j];
				}
				Constraint *cn=allocArrayConstraint(OR, index, array);
				VectorConstraint *vec=simplifyConstraint(cn);
				internalfreeConstraint(this);
				return vec;
			}
			case VAR:
			case NOTVAR:
				break;
			case OR: {
				uint nsize=this->numoperandsorvar+c->numoperandsorvar-1;
				Constraint *array[nsize];
				uint index=0;
				for(uint j=0;j<this->numoperandsorvar;j++)
					if (j!=i)
						array[index++]=this->operands[j];
				for(uint j=0;j<c->numoperandsorvar;j++)
					array[index++]=c->operands[j];
				Constraint *cn=allocArrayConstraint(OR, nsize, array);
				VectorConstraint *vec=simplifyConstraint(cn);
				internalfreeConstraint(this);
				internalfreeConstraint(c);
				return vec;
			}
			case IMPLIES: {
				uint nsize=this->numoperandsorvar+1;
				Constraint *array[nsize];
				uint index=0;
				for(uint j=0;j<this->numoperandsorvar;j++)
					if (j!=i)
						array[index++]=this->operands[j];
				array[index++]=negateConstraint(c->operands[0]);
				array[index++]=c->operands[1];
				Constraint *cn=allocArrayConstraint(OR, nsize, array);
				VectorConstraint *vec=simplifyConstraint(cn);
				internalfreeConstraint(this);
				internalfreeConstraint(c);
				return vec;
			}
			case AND: {
				Constraint *array[this->numoperandsorvar];

				VectorConstraint *vec=allocDefVectorConstraint();
				for(uint j=0;j<c->numoperandsorvar;j++) {
					//copy other elements
					for(uint k=0;k<this->numoperandsorvar;k++) {
						if (k!=i) {
							array[k]=cloneConstraint(this->operands[k]);
						}
					}

					array[i]=cloneConstraint(c->operands[j]);
					Constraint *cn=allocArrayConstraint(OR, this->numoperandsorvar, array);
					VectorConstraint * newvec=simplifyConstraint(cn);
					if (mergeandfree(vec, newvec)) {
						freerecConstraint(this);
						return vec;
					}
				}
				freerecConstraint(this);
				return vec;
			}
			default:
				ASSERT(0);
			}
			//continue on to next item
		}
		VectorConstraint * vec=allocDefVectorConstraint();
		if (this->numoperandsorvar==1) {
			Constraint *c=this->operands[0];
			freerecConstraint(this);
			pushVectorConstraint(vec, c);
		} else
			pushVectorConstraint(vec, this);
		return vec;
	}
	case IMPLIES: {
		Constraint *cn=allocConstraint(OR, negateConstraint(this->operands[0]), this->operands[1]);
		VectorConstraint * vec=simplifyConstraint(cn);
		internalfreeConstraint(this);
		return vec;
	}
	default:
		ASSERT(0);
		return NULL;
	}
}

Constraint * negateConstraint(Constraint * this) {
	switch(this->type) {
	case TRUE:
		return &cfalse;
	case FALSE:
		return &ctrue;
	case NOTVAR:
	case VAR:
		return this->neg;
	case IMPLIES: {
		Constraint *l=this->operands[0];
		Constraint *r=this->operands[1];
		this->operands[0]=r;
		this->operands[1]=l;
		return this;
	}
	case AND:
	case OR: {
		for(uint i=0;i<this->numoperandsorvar;i++) {
			this->operands[i]=negateConstraint(this->operands[i]);
		}
		this->type=(this->type==AND) ? OR : AND;
		return this;
	}
	default:
		ASSERT(0);
		return NULL;
	}
}
