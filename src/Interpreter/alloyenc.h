#ifndef ALLOYENC_H
#define ALLOYENC_H

#include "classlist.h"
#include "signatureenc.h"
#include "interpreter.h"
#include <iostream>
#include <fstream>

class AlloyEnc: public Interpreter{
public:
	AlloyEnc(CSolver *solver);
	virtual ~AlloyEnc();
protected:
	virtual void dumpFooter();
	virtual void dumpHeader();
	virtual void compileRunCommand(char * command , size_t size);
	virtual int getResult();
	virtual void dumpAllConstraints(Vector<char *> &facts);
	virtual string negateConstraint(string constr);
	virtual string encodeBooleanLogic( BooleanLogic *bl);
	virtual string encodeBooleanVar( BooleanVar *bv);
	string encodeOperatorPredicate(BooleanPredicate *constraint);
	virtual string processElementFunction(ElementFunction *element, ElementSig *signature);
	virtual string operatorPredicateConstraint(CompOp op, ElementSig *elemSig1, ElementSig *elemSig2);
};

#endif
