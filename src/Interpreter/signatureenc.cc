#include "signatureenc.h"
#include "element.h"
#include "set.h"
#include "signature.h"
#include "interpreter.h"

SignatureEnc::SignatureEnc(Interpreter *inter) :
	interpreter(inter),
	maxValue(0)
{
}

SignatureEnc::~SignatureEnc() {
	for (uint i = 0; i < signatures.getSize(); i++) {
		Signature *s = signatures.get(i);
		delete s;
	}
}

void SignatureEnc::updateMaxValue(Set *set) {
	for (uint i = 0; i < set->getSize(); i++) {
		if (set->getElement(i) > maxValue) {
			maxValue = set->getElement(i);
		}
	}
}

ValuedSignature *SignatureEnc::getBooleanSignature(Boolean *bvar) {
	ValuedSignature *bsig = (ValuedSignature *)encoded.get((void *)bvar);
	if (bsig == NULL) {
		bsig = interpreter->getBooleanSignature(getUniqueSigID());
		encoded.put(bvar, bsig);
		signatures.push(bsig);
		interpreter->writeToFile(bsig->getSignature());
	}
	return bsig;
}

ValuedSignature *SignatureEnc::getElementSignature(Element *element) {
	ValuedSignature *esig = (ValuedSignature *)encoded.get((void *)element);
	if (esig == NULL) {
		Set *set = element->getRange();
		Signature *ssig = (Signature *)encoded.get((void *)set);
		if (ssig == NULL) {
			ssig = interpreter->getSetSignature(getUniqueSigID(), set);
			encoded.put(set, ssig);
			signatures.push(ssig);
			interpreter->writeToFile(ssig->getSignature());
			updateMaxValue(set);
		}
		esig = interpreter->getElementSignature(getUniqueSigID(), ssig);
		encoded.put(element, esig);
		signatures.push(esig);
		interpreter->writeToFile(esig->getSignature());

	}
	return esig;
}

void SignatureEnc::setValue(uint id, uint value) {
	ValuedSignature *sig = getValuedSignature(id);
	ASSERT(sig != NULL);
	sig->setValue(value);
}

int SignatureEnc::getValue(void *astnode) {
	ValuedSignature *sig = (ValuedSignature *)encoded.get(astnode);
	ASSERT(sig != NULL);
	return sig->getValue();
}
