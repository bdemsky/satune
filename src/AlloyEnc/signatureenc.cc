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

BooleanSig *SignatureEnc::getBooleanSignature(Boolean *bvar){
	BooleanSig *bsig = (BooleanSig *)encoded.get((void *)bvar);
	if(bsig == NULL){
		bsig = new BooleanSig(getUniqueSigID());
		encoded.put(bvar, bsig);
		signatures.push(bsig);
		alloyEncoder->writeToFile(bsig->getSignature());
	}
	return bsig;
}

ElementSig *SignatureEnc::getElementSignature(Element *element){
	ElementSig *esig = (ElementSig *)encoded.get((void *)element);
	if(esig == NULL){
		Set *set = element->getRange();
		SetSig *ssig = (SetSig *)encoded.get((void *)set);
		if(ssig == NULL){
			ssig = new SetSig(getUniqueSigID(), set);
			encoded.put(set, ssig);
			signatures.push(ssig);
			alloyEncoder->writeToFile(ssig->getSignature());
			updateMaxValue(set);
		}
		esig = new ElementSig(getUniqueSigID(), ssig);
		encoded.put(element, esig);
		signatures.push(esig);
		alloyEncoder->writeToFile(esig->getSignature());

	}
	return esig;
}

void SignatureEnc::setValue(uint id, uint value){
	ValuedSignature *sig = getValuedSignature(id);
	ASSERT(sig != NULL);
	sig->setValue(value);
}

int SignatureEnc::getValue(void *astnode){
	ValuedSignature *sig = (ValuedSignature *)encoded.get(astnode);
	ASSERT(sig != NULL);
	return sig->getValue();
}
