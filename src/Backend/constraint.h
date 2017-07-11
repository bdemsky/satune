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
#include "structs.h"

enum ConstraintType {
	TRUE, FALSE, IMPLIES, AND, OR, VAR, NOTVAR, BOGUS
};

typedef enum ConstraintType CType;

struct Constraint {
	CType type;
	uint numoperandsorvar;
	Constraint ** operands;
	Constraint *neg;
};

Constraint * allocConstraint(CType t, Constraint *l, Constraint *r);
Constraint * allocUnaryConstraint(CType t, Constraint *l);
Constraint * allocArrayConstraint(CType t, uint num, Constraint ** array);
Constraint * allocVarConstraint(CType t, uint var);

void deleteConstraint(Constraint *);
void printConstraint(Constraint * c);
void dumpConstraint(Constraint * c, IncrementalSolver *solver);
static inline uint getVarConstraint(Constraint * c) {ASSERT(c->type==VAR); return c->numoperandsorvar;}
VectorConstraint * simplifyConstraint(Constraint * This);
static inline CType getType(Constraint * c) {return c->type;}
static inline bool isFalse(Constraint * c) {return c->type==FALSE;}
static inline bool isTrue(Constraint * c) {return c->type==TRUE;}
void internalfreeConstraint(Constraint * c);
void freerecConstraint(Constraint * c);
Constraint * cloneConstraint(Constraint * c);
static inline void setNegConstraint(Constraint * This, Constraint *c) {This->neg=c;}
Constraint *negateConstraint(Constraint * c);


extern Constraint ctrue;
extern Constraint cfalse;

Constraint * generateBinaryConstraint(uint numvars, Constraint ** vars, uint value);
Constraint * generateLTConstraint(uint numvars, Constraint ** vars, uint value);
Constraint * generateEquivNVConstraint(uint numvars, Constraint **var1, Constraint **var2);
Constraint * generateEquivConstraint(Constraint *var1, Constraint *var2);


#endif
