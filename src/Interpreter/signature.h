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

class ValuedSignature: public Signature{
public:
	ValuedSignature(uint id);
	int getValue();
	void setValue(int v){value = v;}
protected:
	int value;
};

class BooleanSig: public ValuedSignature{
public:
	BooleanSig(uint id);
	virtual ~BooleanSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
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

class ElementSig: public ValuedSignature{
public:
	ElementSig(uint id, SetSig *ssig);
	virtual ~ElementSig(){}
	virtual string toString() const;
	virtual string getAbsSignature() const;
	virtual string getSignature() const;
private:
	SetSig *ssig;
	static bool encodeAbs;
};

string operator+(const string& str, const Signature& sig);

#endif
