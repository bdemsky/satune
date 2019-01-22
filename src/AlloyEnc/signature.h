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
	virtual string getSignature() const =0;
	virtual ~Signature(){}
protected:
	uint id;
};

class SetSig: public Signature{
public:
	SetSig(uint id, Set *set);
	virtual ~SetSig(){}
	virtual string toString() const;
	virtual string getSignature() const;
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
	virtual string getSignature() const;
private:
	SetSig *ssig;
	uint64_t value;
};

string operator+(const string& str, const Signature& sig);

#endif
