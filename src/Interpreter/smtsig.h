#ifndef SMTSIG_H
#define SMTSIG_H
#include <string>
#include <iostream>
#include "signature.h"
#include "classlist.h"
using namespace std;

class SMTBoolSig : public ValuedSignature {
public:
	SMTBoolSig(uint id);
	virtual ~SMTBoolSig() {}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
};

class SMTSetSig : public Signature {
public:
	SMTSetSig(uint id, Set *set);
	virtual ~SMTSetSig() {}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
	string constraint;
};

class SMTElementSig : public ValuedSignature {
public:
	SMTElementSig(uint id, SMTSetSig *ssig);
	virtual ~SMTElementSig() {}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
	SMTSetSig *ssig;
};

#endif
