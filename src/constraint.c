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

Constraint ctrue(TRUE);
Constraint cfalse(FALSE);

Constraint::Constraint(CType t, Constraint *l, Constraint *r) :
	type(t),
	numoperandsorvar(2),
	operands((Constraint **)model_malloc(2*sizeof(Constraint *))),
	neg(NULL)
{
	ASSERT(l!=NULL);
	//if (type==IMPLIES) {
	//type=OR;
	//	operands[0]=l->negate();
	//	} else {
	operands[0]=l;
	//	}
	operands[1]=r;
}

Constraint::Constraint(CType t, Constraint *l) :
	type(t),
	numoperandsorvar(1),
	operands((Constraint **)model_malloc(sizeof(Constraint *))),
	neg(NULL)
{
	operands[0]=l;
}

Constraint::Constraint(CType t, uint num, Constraint **array) :
	type(t),
	numoperandsorvar(num),
	operands((Constraint **)model_malloc(num*sizeof(Constraint *))),
	neg(NULL)
{
	memcpy(operands, array, num*sizeof(Constraint *));
}

Constraint::Constraint(CType t) :
	type(t),
	numoperandsorvar(0xffffffff),
	operands(NULL),
	neg(NULL)
{
}

Constraint::Constraint(CType t, uint v) :
	type(t),
	numoperandsorvar(v),
	operands(NULL),
	neg(NULL)
{
}

Constraint::~Constraint() {
	if (operands!=NULL)
		model_free(operands);
}

void Constraint::dumpConstraint(IncrementalSolver *solver) {
	if (type==VAR) {
		solver->addClauseLiteral(numoperandsorvar);
		solver->addClauseLiteral(0);
	} else if (type==NOTVAR) {
		solver->addClauseLiteral(-numoperandsorvar);
		solver->addClauseLiteral(0);
	} else {
		ASSERT(type==OR);
		for(uint i=0;i<numoperandsorvar;i++) {
			Constraint *c=operands[i];
			if (c->type==VAR) {
				solver->addClauseLiteral(c->numoperandsorvar);
			} else if (c->type==NOTVAR) {
				solver->addClauseLiteral(-c->numoperandsorvar);
			} else ASSERT(0);
		}
		solver->addClauseLiteral(0);
	}
}

void Constraint::free() {
	switch(type) {
	case TRUE:
	case FALSE:
	case NOTVAR:
	case VAR:
		return;
	case BOGUS:
		ASSERT(0);
	default:
		type=BOGUS;
		delete this;
	}
}

void Constraint::freerec() {
	switch(type) {
	case TRUE:
	case FALSE:
	case NOTVAR:
	case VAR:
		return;
	case BOGUS:
		ASSERT(0);
	default:
		if (operands!=NULL) {
			for(uint i=0;i<numoperandsorvar;i++)
				operands[i]->freerec();
		}
		type=BOGUS;
		delete this;
	}
}


void Constraint::print() {
	switch(type) {
	case TRUE:
		model_print("true");
		break;
	case FALSE:
		model_print("false");
		break;
	case IMPLIES:
		model_print("(");
		operands[0]->print();
		model_print(")");
		model_print("=>");
		model_print("(");
		operands[1]->print();
		model_print(")");
		break;
	case AND:
	case OR:
		model_print("(");
		for(uint i=0;i<numoperandsorvar;i++) {
			if (i!=0) {
				if (type==AND)
					model_print(" ^ ");
				else
					model_print(" v ");
			}
			operands[i]->print();
		}
		model_print(")");
		break;
	case VAR:
		model_print("t%u",numoperandsorvar);
		break;
	case NOTVAR:
		model_print("!t%u",numoperandsorvar);
		break;
	default:
		ASSERT(0);
	}
}

Constraint * Constraint::clone() {
	switch(type) {
	case TRUE:
	case FALSE:
	case VAR:
	case NOTVAR:
		return this;
	case IMPLIES:
		return new Constraint(IMPLIES, operands[0]->clone(), operands[1]->clone());
	case AND:
	case OR: {
		Constraint *array[numoperandsorvar];
		for(uint i=0;i<numoperandsorvar;i++) {
			array[i]=operands[i]->clone();
		}
		return new Constraint(type, numoperandsorvar, array);
	}
	default:
		ASSERT(0);
		return NULL;
	}
}

Constraint * generateConstraint(uint numvars, Constraint ** vars, uint value) {
	Constraint *carray[numvars];
	for(uint j=0;j<numvars;j++) {
		carray[j]=((value&1)==1) ? vars[j] : vars[j]->negate();
		value=value>>1;
	}

	return new Constraint(AND, numvars, carray);
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
				orarray[ori++]=vars[j]->negate();
			val=val>>1;
		}
		//no ones to flip, so bail now...
		if (ori==0) {
			return new Constraint(AND, andi, andarray);
		}
		andarray[andi++]=new Constraint(OR, ori, orarray);

		value=value+(1<<(__builtin_ctz(value)));
		//flip the last one
	}
}

Constraint * generateEquivConstraint(uint numvars, Constraint **var1, Constraint **var2) {
	if (numvars==0)
		return &ctrue;
	Constraint *array[numvars*2];
	for(uint i=0;i<numvars;i++) {
		array[i*2]=new Constraint(OR, var1[i]->clone()->negate(), var2[i]);
		array[i*2+1]=new Constraint(OR, var1[i], var2[i]->clone()->negate());
	}
	return new Constraint(AND, numvars*2, array);
}

Constraint * generateEquivConstraint(Constraint *var1, Constraint *var2) {
	Constraint * imp1=new Constraint(OR, var1->clone()->negate(), var2);
	Constraint * imp2=new Constraint(OR, var1, var2->clone()->negate());

	return new Constraint(AND, imp1, imp2);
}

bool mergeandfree(ModelVector<Constraint *> * to, ModelVector<Constraint *> * from) {
	for(uint i=0;i<from->size();i++) {
		Constraint *c=(*from)[i];
		if (c->type==TRUE)
			continue;
		if (c->type==FALSE) {
			for(uint j=i+1;j<from->size();j++)
				(*from)[j]->freerec();
			for(uint j=0;j<to->size();j++)
				(*to)[j]->freerec();
			to->clear();
			to->push_back(&ctrue);
			delete from;
			return true;
		}
		to->push_back(c);
	}
	delete from;
	return false;
}

ModelVector<Constraint *> * Constraint::simplify() {
	switch(type) {
	case TRUE:
	case VAR:
	case NOTVAR:
	case FALSE: {
		ModelVector<Constraint *> *vec=new ModelVector<Constraint *>();
		vec->push_back(this);
		return vec;
	}
	case AND: {
		ModelVector<Constraint *> *vec=new ModelVector<Constraint *>();
		for(uint i=0;i<numoperandsorvar;i++) {
			ModelVector<Constraint *> *subvec=operands[i]->simplify();
			if (mergeandfree(vec, subvec)) {
				for(uint j=i+1;j<numoperandsorvar;j++) {
					operands[j]->freerec();
				}
				this->free();
				return vec;
			}
		}
		this->free();
		return vec;
	}
	case OR: {
		for(uint i=0;i<numoperandsorvar;i++) {
			Constraint *c=operands[i];
			switch(c->type) {
			case TRUE: {
				ModelVector<Constraint *> *vec=new ModelVector<Constraint *>();
				vec->push_back(c);
				this->freerec();
				return vec;
			}
			case FALSE: {
				Constraint *array[numoperandsorvar-1];
				uint index=0;
				for(uint j=0;j<numoperandsorvar;j++) {
					if (j!=i)
						array[index++]=operands[j];
				}
				Constraint *cn=new Constraint(OR, index, array);
				ModelVector<Constraint *> *vec=cn->simplify();
				this->free();
				return vec;
			}
			case VAR:
			case NOTVAR:
				break;
			case OR: {
				uint nsize=numoperandsorvar+c->numoperandsorvar-1;
				Constraint *array[nsize];
				uint index=0;
				for(uint j=0;j<numoperandsorvar;j++)
					if (j!=i)
						array[index++]=operands[j];
				for(uint j=0;j<c->numoperandsorvar;j++)
					array[index++]=c->operands[j];
				Constraint *cn=new Constraint(OR, nsize, array);
				ModelVector<Constraint *> *vec=cn->simplify();
				this->free();
				c->free();
				return vec;
			}
			case IMPLIES: {
				uint nsize=numoperandsorvar+1;
				Constraint *array[nsize];
				uint index=0;
				for(uint j=0;j<numoperandsorvar;j++)
					if (j!=i)
						array[index++]=operands[j];
				array[index++]=c->operands[0]->negate();
				array[index++]=c->operands[1];
				Constraint *cn=new Constraint(OR, nsize, array);
				ModelVector<Constraint *> *vec=cn->simplify();
				this->free();
				c->free();
				return vec;
			}
			case AND: {
				Constraint *array[numoperandsorvar];

				ModelVector<Constraint *> *vec=new ModelVector<Constraint *>();
				for(uint j=0;j<c->numoperandsorvar;j++) {
					//copy other elements
					for(uint k=0;k<numoperandsorvar;k++) {
						if (k!=i) {
							array[k]=operands[k]->clone();
						}
					}

					array[i]=c->operands[j]->clone();
					Constraint *cn=new Constraint(OR, numoperandsorvar, array);
					ModelVector<Constraint *> * newvec=cn->simplify();
					if (mergeandfree(vec, newvec)) {
						this->freerec();
						return vec;
					}
				}
				this->freerec();
				return vec;
			}
			default:
				ASSERT(0);
			}
			//continue on to next item
		}
		ModelVector<Constraint *> *vec=new ModelVector<Constraint *>();
		if (numoperandsorvar==1) {
			Constraint *c=operands[0];
			this->freerec();
			vec->push_back(c);
		} else
			vec->push_back(this);
		return vec;
	}
	case IMPLIES: {
		Constraint *cn=new Constraint(OR, operands[0]->negate(), operands[1]);
		ModelVector<Constraint *> *vec=cn->simplify();
		this->free();
		return vec;
	}
	default:
		ASSERT(0);
		return NULL;
	}
}

Constraint * Constraint::negate() {
	switch(type) {
	case TRUE:
		return &cfalse;
	case FALSE:
		return &ctrue;
	case NOTVAR:
	case VAR:
		return this->neg;
	case IMPLIES: {
		Constraint *l=operands[0];
		Constraint *r=operands[1];
		operands[0]=r;
		operands[1]=l;
		return this;
	}
	case AND:
	case OR: {
		for(uint i=0;i<numoperandsorvar;i++) {
			operands[i]=operands[i]->negate();
		}
		type=(type==AND) ? OR : AND;
		return this;
	}
	default:
		ASSERT(0);
		return NULL;
	}
}
