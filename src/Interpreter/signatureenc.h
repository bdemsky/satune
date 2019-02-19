#ifndef SIGNATUREENC_H
#define SIGNATUREENC_H

#include "classlist.h"
#include "structs.h"
#include "cppvector.h"

class SignatureEnc {
public:
	SignatureEnc(Interpreter *_alloyEncoder);
	~SignatureEnc();
	void setValue(uint id, uint value);
	ElementSig *getElementSignature(Element *element);
	BooleanSig *getBooleanSignature(Boolean *bvar);
	int getAlloyIntScope();
	int getValue(void *astnode);
private:
	ValuedSignature *getValuedSignature(uint uniqueID){return (ValuedSignature*)signatures.get(uniqueID-1);}
	uint getUniqueSigID(){return signatures.getSize() +1;}
	void updateMaxValue(Set *set);
	CloneMap encoded;
	Vector<Signature*> signatures;
	Interpreter *interpreter;
	uint64_t maxValue;
};
#endif
