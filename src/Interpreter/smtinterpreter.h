#ifndef SMTINTERPRETER_H
#define SMTINTERPRETER_H

#include "classlist.h"
#include "signatureenc.h"
#include "interpreter.h"
#include <iostream>
#include <fstream>

class SMTInterpreter: public Interpreter{
public:
	SMTInterpreter(CSolver *solver);
	virtual ValuedSignature *getBooleanSignature(uint id);
	virtual ValuedSignature *getElementSignature(uint id, Signature *ssig);
	virtual Signature *getSetSignature(uint id, Set *set);
	virtual ~SMTInterpreter();
protected:
	virtual void dumpFooter();
	virtual void dumpHeader();
	virtual void compileRunCommand(char * command , size_t size);
	virtual int getResult();
	virtual void dumpAllConstraints(Vector<char *> &facts);
	virtual string negateConstraint(string constr);
	virtual string encodeBooleanLogic( BooleanLogic *bl);
	virtual string encodeBooleanVar( BooleanVar *bv);
	void extractValue(char *idline, char *valueline);
	virtual string processElementFunction(ElementFunction *element, ValuedSignature *signature);
	virtual string operatorPredicateConstraint(CompOp op, ValuedSignature *elemSig1, ValuedSignature *elemSig2);
};

#endif
