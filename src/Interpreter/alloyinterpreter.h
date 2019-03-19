#ifndef ALLOYINTERPRETER_H
#define ALLOYINTERPRETER_H

#include "classlist.h"
#include "signatureenc.h"
#include "interpreter.h"
#include <iostream>
#include <fstream>

class AlloyInterpreter : public Interpreter {
public:
	AlloyInterpreter(CSolver *solver);
	virtual ValuedSignature *getBooleanSignature(uint id);
	virtual ValuedSignature *getElementSignature(uint id, Signature *ssig);
	virtual Signature *getSetSignature(uint id, Set *set);
	virtual ~AlloyInterpreter();
protected:
	virtual void dumpFooter();
	virtual void dumpHeader();
	int getAlloyIntScope();
	virtual void compileRunCommand(char *command, size_t size);
	virtual int getResult();
	virtual void dumpAllConstraints(Vector<char *> &facts);
	virtual string negateConstraint(string constr);
	virtual string encodeBooleanLogic( BooleanLogic *bl);
	virtual string encodeBooleanVar( BooleanVar *bv);
	string encodeOperatorPredicate(BooleanPredicate *constraint);
	virtual string processElementFunction(ElementFunction *element, ValuedSignature *signature);
	virtual string operatorPredicateConstraint(CompOp op, ValuedSignature *elemSig1, ValuedSignature *elemSig2);
};

#endif
