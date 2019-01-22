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
	uint64_t getValue(Element *element);
private:
	CloneMap encoded;
	Vector<Signature*> signatures;
	AlloyEnc *alloyEncoder;
};
#endif
