#include "smtsig.h"
#include "set.h"

SMTBoolSig::SMTBoolSig(uint id):
	ValuedSignature(id)
{
}

string SMTBoolSig::toString() const{
	return "b" + to_string(id);
}

string SMTBoolSig::getSignature() const{
	return "(declare-const b" + to_string(id) + " Bool)";
}

string SMTBoolSig::getAbsSignature() const{
	return "";
}

SMTElementSig::SMTElementSig(uint id, SMTSetSig *_ssig): 
	ValuedSignature(id),
	ssig(_ssig)
{
}

string SMTElementSig::toString() const{
	return "e" + to_string(id);
}

string SMTElementSig::getSignature() const{
	string str = "(declare-const e" + to_string(id) + " Int)\n";
	string constraint = ssig->getAbsSignature();
	size_t start_pos;
	string placeholder = "$";
	while((start_pos = constraint.find(placeholder)) != string::npos){
		constraint.replace(start_pos, placeholder.size(), to_string(id));
	}
	//constraint.replace(constraint.begin(), constraint.end(), "$", );
	str += constraint;
	return str;
}

string SMTElementSig::getAbsSignature() const{
	return "";
	
}

SMTSetSig::SMTSetSig(uint id, Set *set): Signature(id){
	ASSERT(set->getSize() > 0);
	int min = set->getElement(0), max = set->getElement(set->getSize()-1);
	Vector<int> holes;
	int prev = set->getElement(0);
	for(uint i=1; i< set->getSize(); i++){
		int curr = set->getElement(i);
		if(prev != curr -1){
			for(int j=prev+1; j< curr; j++){
				holes.push(j);
			}
		}
		prev = curr;
	}
	constraint = "(assert (<= e$ " + to_string(max) +"))\n";
	constraint += "(assert (>= e$ " + to_string(min) +"))\n";
	for(uint i=0; i< holes.getSize(); i++){
		constraint += "(assert (not (= e$ " + to_string(holes.get(i)) +")))\n";
	}
}

string SMTSetSig::toString() const{
	return "";
}

string SMTSetSig::getSignature() const{
	return "";
}

string SMTSetSig::getAbsSignature() const{
	return constraint;
	
}
