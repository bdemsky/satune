/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#ifndef CONSTRAINT_H
#define CONSTRAINT_H
#include "classlist.h"
#include "stl-model.h"

enum ConstraintType {
	TRUE, FALSE, IMPLIES, AND, OR, VAR, NOTVAR, BOGUS
};

typedef enum ConstraintType CType;

class Constraint {
public:
	Constraint(CType t, Constraint *l, Constraint *r);
	Constraint(CType t, Constraint *l);
	Constraint(CType t, uint num, Constraint ** array);
	Constraint(CType t, uint var);
	Constraint(CType t);
	~Constraint();
	void print();
	void dumpConstraint(IncrementalSolver *solver);
	uint getVar() {ASSERT(type==VAR); return numoperandsorvar;}
	ModelVector<Constraint *> * simplify();
	CType getType() {return type;}
	bool isFalse() {return type==FALSE;}
	bool isTrue() {return type==TRUE;}
	void free();
	void freerec();
	Constraint * clone();
	void setNeg(Constraint *c) {neg=c;}
	Constraint *negate();

	MEMALLOC;
private:
	CType type;
	uint numoperandsorvar;
	Constraint ** operands;
	Constraint *neg;
	friend bool mergeandfree(ModelVector<Constraint *> * to, ModelVector<Constraint *> * from);
};

extern Constraint ctrue;
extern Constraint cfalse;

Constraint * generateConstraint(uint numvars, Constraint ** vars, uint value);
Constraint * generateLTConstraint(uint numvars, Constraint ** vars, uint value);
Constraint * generateEquivConstraint(uint numvars, Constraint **var1, Constraint **var2);
Constraint * generateEquivConstraint(Constraint *var1, Constraint *var2);
#endif
