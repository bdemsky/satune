#include "signature.h"
#include "set.h"

ValuedSignature::ValuedSignature(uint id): 
	Signature(id), 
	value(-1) 
{
}

int ValuedSignature::getValue(){
	ASSERT(value != -1);
	return value;
}

string Signature::operator+(const string& str){
	return toString() + str;
}

string operator+(const string& str, const Signature& sig){
        return str + sig.toString();
}
