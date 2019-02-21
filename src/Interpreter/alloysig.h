#ifndef ALLOYSIG_H
#define ALLOYSIG_H
#include <string>
#include <iostream>
#include "signature.h"
#include "classlist.h"
using namespace std;

class AlloyBoolSig: public ValuedSignature{
public:
	AlloyBoolSig(uint id);
	virtual ~AlloyBoolSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
	static bool encodeAbs;
};

class AlloySetSig: public Signature{
public:
	AlloySetSig(uint id, Set *set);
	virtual ~AlloySetSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
	static bool encodeAbs;
private:
	string domain;
};

class AlloyElementSig: public ValuedSignature{
public:
	AlloyElementSig(uint id, Signature *ssig);
	virtual ~AlloyElementSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
	Signature *ssig;
	static bool encodeAbs;
};

#endif
