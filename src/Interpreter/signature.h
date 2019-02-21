#ifndef SIGNATURE_H
#define SIGNATURE_H
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

string operator+(const string& str, const Signature& sig);

#endif
