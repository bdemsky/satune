#ifndef SIGNATUREENC_H
#define SIGNATUREENC_H

#include "classlist.h"
#include "structs.h"
#include "cppvector.h"

class SignatureEnc {
public:
	SignatureEnc(AlloyEnc *_alloyEncoder);
	~SignatureEnc();
	void setValue(uint id, uint64_t value);
	ElementSig *getElementSignature(Element *element);
	int getAlloyIntScope();
	uint64_t getValue(Element *element);
private:
	void updateMaxValue(Set *set);
	CloneMap encoded;
	Vector<Signature*> signatures;
	AlloyEnc *alloyEncoder;
	uint64_t maxValue;
};
#endif
