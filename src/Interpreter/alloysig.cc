#include "alloysig.h"
#include "set.h"

bool AlloyBoolSig::encodeAbs = true;
bool AlloySetSig::encodeAbs = true;
bool AlloyElementSig::encodeAbs = true;

AlloyBoolSig::AlloyBoolSig(uint id):
	ValuedSignature(id)
{
}

string AlloyBoolSig::toString() const{
	return "Boolean" + to_string(id) + ".value";
}

string AlloyBoolSig::getSignature() const{
	string str;
	if(encodeAbs){
		encodeAbs = false;
		str += getAbsSignature();
	}
	str += "one sig Boolean" + to_string(id) + " extends AbsBool {}";
	return str;
}

string AlloyBoolSig::getAbsSignature() const{
	string str;
	if(AlloySetSig::encodeAbs){
		AlloySetSig::encodeAbs = false;
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

AlloyElementSig::AlloyElementSig(uint id, Signature *_ssig): 
	ValuedSignature(id),
	ssig(_ssig)
{
}

string AlloyElementSig::toString() const{
	return "Element" + to_string(id) + ".value";
}

string AlloyElementSig::getSignature() const{
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

string AlloyElementSig::getAbsSignature() const{
	return "abstract sig AbsElement {\n\
		value: Int\n\
		}\n";
	
}

AlloySetSig::AlloySetSig(uint id, Set *set): Signature(id){
	ASSERT(set->getSize() > 0);
	domain = to_string(set->getElement(0));
	for(uint i=1; i< set->getSize(); i++){
		domain += " + " + to_string(set->getElement(i));
	}
}

string AlloySetSig::toString() const{
	return "Set" + to_string(id) + ".domain";
}

string AlloySetSig::getSignature() const{
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

string AlloySetSig::getAbsSignature() const{
	return "abstract sig AbsSet {\n\
		domain: set Int\n\
		}\n";
	
}
