#include "signatureenc.h"
#include "element.h"
#include "set.h"
#include "signature.h"
#include "alloyenc.h"
#include "math.h"

SignatureEnc::SignatureEnc(AlloyEnc *ae): 
	alloyEncoder(ae),
	maxValue(0)
{
}

SignatureEnc::~SignatureEnc(){
	for(uint i=0; i<signatures.getSize(); i++){
		Signature *s = signatures.get(i);
		delete s;
	}
}

int SignatureEnc::getAlloyIntScope(){
	double mylog = log2(maxValue + 1);
	return floor(mylog) == mylog ? (int)mylog + 1: (int)mylog + 2;
}

void SignatureEnc::updateMaxValue(Set *set){
	for(uint i=0; i< set->getSize(); i++){
		if(set->getElement(i) > maxValue){
			maxValue = set->getElement(i);
		}
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
			updateMaxValue(set);
		}
		esig = new ElementSig(signatures.getSize(), ssig);
		element->print();
		model_print(" = Element%u\n", signatures.getSize());
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
	model_print("******************\n");
	element->print();
	model_print("Value = %" PRId64 "\n", sig->getValue());
	return sig->getValue();
}
