#include "signatureenc.h"
#include "element.h"
#include "set.h"
#include "signature.h"
#include "alloyenc.h"

SignatureEnc::SignatureEnc(AlloyEnc *ae): alloyEncoder(ae){
}

SignatureEnc::~SignatureEnc(){
	for(uint i=0; i<signatures.getSize(); i++){
		Signature *s = signatures.get(i);
		delete s;
	}
}

ElementSig *SignatureEnc::getElementSignature(Element *element){
	ElementSig *esig = (ElementSig *)encoded.get((void *)element);
	if(esig == NULL){
		Set *set = element->getRange();
		SetSig *ssig = (SetSig *)encoded.get((void *)set);
		if(ssig == NULL){
			ssig = new SetSig(signatures.getSize(), set);
			encoded.put(set, ssig);
			signatures.push(ssig);
			alloyEncoder->writeToFile(ssig->getSignature());
		}
		esig = new ElementSig(signatures.getSize(), ssig);
		encoded.put(element, esig);
		signatures.push(esig);
		alloyEncoder->writeToFile(esig->getSignature());

	}
	return esig;
}

void SignatureEnc::setValue(uint id, uint64_t value){
	ElementSig *sig = (ElementSig *)signatures.get(id);
	sig->setValue(value);
}

uint64_t SignatureEnc::getValue(Element *element){
	ElementSig *sig = (ElementSig *)encoded.get((void *) element);
	ASSERT(sig != NULL);
	return sig->getValue();
}
