#include "signature.h"
#include "set.h"

bool BooleanSig::encodeAbs = true;
bool SetSig::encodeAbs = true;
bool ElementSig::encodeAbs = true;

BooleanSig::BooleanSig(uint id):
	Signature(id),
	value(-1)
{
}

bool BooleanSig::getValue(){
	ASSERT(value != -1);
	return (bool) value;
}

string BooleanSig::toString() const{
	return "Boolean" + to_string(id) + ".value";
}

string BooleanSig::getSignature() const{
	string str;
	if(encodeAbs){
		encodeAbs = false;
		str += getAbsSignature();
	}
	str += "one sig Boolean" + to_string(id) + " extends AbsBool {}";
	return str;
}

string BooleanSig::getAbsSignature() const{
	string str;
	if(SetSig::encodeAbs){
		SetSig::encodeAbs = false;
		str += "abstract sig AbsSet {\
		domain: set Int\
		}\n";
	}
	str +="one sig BooleanSet extends AbsSet {}{\n\
	domain = 0 + 1 \n\
	}\n\
	abstract sig AbsBool {\
	value: Int\
	}{\n\
	value in BooleanSet.domain\n\
	}\n";
	return str;
}

ElementSig::ElementSig(uint id, SetSig *_ssig): 
	Signature(id),
	ssig(_ssig),
	value(0)
{
}

string ElementSig::toString() const{
	return "Element" + to_string(id) + ".value";
}

string ElementSig::getSignature() const{
	string str;
	if(encodeAbs){
		encodeAbs = false;
		str += getAbsSignature();
	}
	str += "one sig Element" + to_string(id) + " extends AbsElement {}{\n\
		value in " + *ssig + "\n\
		}";
	return str;
}

string ElementSig::getAbsSignature() const{
	return "abstract sig AbsElement {\n\
		value: Int\n\
		}\n";
	
}

SetSig::SetSig(uint id, Set *set): Signature(id){
	ASSERT(set->getSize() > 0);
	domain = to_string(set->getElement(0));
	for(uint i=1; i< set->getSize(); i++){
		domain += " + " + to_string(set->getElement(i));
	}
}

string SetSig::toString() const{
	return "Set" + to_string(id) + ".domain";
}

string SetSig::getSignature() const{
	string str;
	if(encodeAbs){
		encodeAbs = false;
		str += getAbsSignature();
	}
	str += "one sig Set" + to_string(id) + " extends AbsSet {}{\n\
		domain = " + domain + "\n\
		}";
	return str;
}

string SetSig::getAbsSignature() const{
	return "abstract sig AbsSet {\n\
		domain: set Int\n\
		}\n";
	
}

string Signature::operator+(const string& str){
	return toString() + str;
}

string operator+(const string& str, const Signature& sig){
        return str + sig.toString();
}
