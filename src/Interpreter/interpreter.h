#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "classlist.h"
#include "signatureenc.h"
#include "signature.h"
#include <iostream>
#include <fstream>
using namespace std;

class Interpreter{
public:
	Interpreter(CSolver *solver);
	void encode();
	int solve();
	void writeToFile(string str);
	uint64_t getValue(Element *element);
	bool getBooleanValue(Boolean *element);
	virtual ValuedSignature *getBooleanSignature(uint id) = 0;
	virtual ValuedSignature *getElementSignature(uint id, Signature *ssig) = 0;
	virtual Signature *getSetSignature(uint id, Set *set) = 0;
	virtual ~Interpreter();
protected:
	virtual void dumpFooter() = 0;
	virtual void dumpHeader() = 0;
	uint getTimeout();
	virtual void compileRunCommand(char * command, size_t size) = 0;
	string encodeConstraint(BooleanEdge constraint);
	virtual int getResult() = 0;
	virtual string negateConstraint(string constr) = 0;
	virtual void dumpAllConstraints(Vector<char *> &facts) = 0;
	virtual string encodeBooleanLogic( BooleanLogic *bl) = 0;
	virtual string encodeBooleanVar( BooleanVar *bv) = 0;
	string encodePredicate( BooleanPredicate *bp);
	string encodeOperatorPredicate(BooleanPredicate *constraint);
	virtual string processElementFunction(ElementFunction *element, ValuedSignature *signature) = 0;
	virtual string operatorPredicateConstraint(CompOp op, ValuedSignature *elemSig1, ValuedSignature *elemSig2) = 0;
	CSolver *csolver;
	SignatureEnc sigEnc;
	ofstream output;
};

#endif
