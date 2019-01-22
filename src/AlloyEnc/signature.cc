#include "signature.h"
#include "set.h"

ElementSig::ElementSig(uint id, SetSig *_ssig): 
	Signature(id),
	ssig(_ssig)
{
	
}

string ElementSig::toString() const{
	return "Element" + to_string(id) + ".value";
}

string ElementSig::getSignature() const{
	return "one sig Element" + to_string(id) + " {\n\
		value: Int\n\
		}{\n\
		value in " + *ssig + "\n\
		}";
	
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
	return "one sig Set" + to_string(id) + " {\n\
		domain: set Int\n\
		}{\n\
		domain = " + domain + "\n\
		}";
	
}

string Signature::operator+(const string& str){
	return toString() + str;
}

string operator+(const string& str, const Signature& sig){
        return str + sig.toString();
}
