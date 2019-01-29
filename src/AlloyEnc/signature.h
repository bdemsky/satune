#ifndef ELEMENTSIG_H
#define ELEMENTSIG_H
#include <string>
#include <iostream>
#include "classlist.h"
using namespace std;

class Signature{
public:
	Signature(uint _id):id(_id){}
	string operator+(const string& s);
	virtual string toString() const = 0;
	virtual string getAbsSignature() const =0;
	virtual string getSignature() const =0;
	virtual ~Signature(){}
protected:
	uint id;
};

class BooleanSig: public Signature{
public:
	BooleanSig(uint id);
	bool getValue();
	void setValue(bool v) {value = v; }
	virtual ~BooleanSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
	int value;
	static bool encodeAbs;
};

class SetSig: public Signature{
public:
	SetSig(uint id, Set *set);
	virtual ~SetSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
	static bool encodeAbs;
private:
	string domain;
};

class ElementSig: public Signature{
public:
	ElementSig(uint id, SetSig *ssig);
	uint64_t getValue() { return value;}
	void setValue(uint64_t v){value = v;}
	virtual ~ElementSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
	SetSig *ssig;
	uint64_t value;
	static bool encodeAbs;
};

string operator+(const string& str, const Signature& sig);

#endif
