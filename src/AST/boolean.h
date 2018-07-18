#ifndef BOOLEAN_H
#define BOOLEAN_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "astops.h"
#include "structs.h"
#include "astnode.h"
#include "functionencoding.h"
#include "constraint.h"
#include "serializer.h"

class Boolean : public ASTNode {
public:
	Boolean(ASTNodeType _type);
	virtual ~Boolean() {}
	virtual Boolean *clone(CSolver *solver, CloneMap *map) = 0;
	virtual void serialize(Serializer * ) = 0;
	virtual void print() = 0;
	virtual bool isTrue() {return boolVal == BV_MUSTBETRUE;}
	virtual bool isFalse() {return boolVal == BV_MUSTBEFALSE;}
	Polarity polarity;
	BooleanValue boolVal;
	Vector<ASTNode *> parents;
	virtual void updateParents() {}
	
	CMEMALLOC;
};

class BooleanConst : public Boolean {
public:
	BooleanConst(bool isTrue);
	Boolean *clone(CSolver *solver, CloneMap *map);
	bool isTrue() {return istrue;}
	bool isFalse() {return !istrue;}
	void serialize(Serializer *serializer ) {};
	virtual void print();
	bool istrue;
	CMEMALLOC;
};

class BooleanVar : public Boolean {
public:
	BooleanVar(VarType t);
	Boolean *clone(CSolver *solver, CloneMap *map);
	void serialize(Serializer *serializer );
	virtual void print();
	VarType vtype;
	Edge var;
	CMEMALLOC;
};

class BooleanOrder : public Boolean {
public:
	BooleanOrder(Order *_order, uint64_t _first, uint64_t _second);
	Boolean *clone(CSolver *solver, CloneMap *map);
	void serialize(Serializer *serializer );
	virtual void print();
	void updateParents();

	Order *order;
	uint64_t first;
	uint64_t second;
	CMEMALLOC;
};

class BooleanPredicate : public Boolean {
public:
	BooleanPredicate(Predicate *_predicate, Element **_inputs, uint _numInputs, BooleanEdge _undefinedStatus);
	Boolean *clone(CSolver *solver, CloneMap *map);
	Predicate *getPredicate() {return predicate;}
	FunctionEncoding *getFunctionEncoding() {return &encoding;}
	void updateParents();
	void serialize(Serializer *serializer );
	virtual void print();
	CMEMALLOC;

	Predicate *predicate;
	FunctionEncoding encoding;
	Array<Element *> inputs;
	BooleanEdge undefStatus;
};

class BooleanLogic : public Boolean {
public:
	BooleanLogic(CSolver *solver, LogicOp _op, BooleanEdge *array, uint asize);
	Boolean *clone(CSolver *solver, CloneMap *map);
	void serialize(Serializer *serializer );
	virtual void print();
	LogicOp op;
	bool replaced;
	Array<BooleanEdge> inputs;
	void updateParents();

	CMEMALLOC;
};
BooleanEdge cloneEdge(CSolver *solver, CloneMap *map, BooleanEdge e);


#endif
