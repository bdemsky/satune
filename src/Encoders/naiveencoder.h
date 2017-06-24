#ifndef NAIVEELEMENTENCODER_H
#define NAIVEELEMENTENCODER_H
#include "classlist.h"

//For now, This function just simply goes through elements/functions and 
//assigns a predefined Encoding to each of them
void assignEncoding(CSolver* csolver);
void encode(CSolver* csolver);
void baseBinaryIndexElementAssign(ElementEncoding *This);
void naiveEncodeFunctionPredicate( FunctionEncoding *This);
void naiveEncodeCircuitFunction(FunctionEncoding* This);
void naiveEncodeEnumeratedFunction(FunctionEncoding* This);
void naiveEncodeEnumTableFunc(ElementFunction* This);
void naiveEncodeEnumOperatingFunc(ElementFunction* This);
#endif
