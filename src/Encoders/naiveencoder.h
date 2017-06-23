#ifndef NAIVEELEMENTENCODER_H
#define NAIVEELEMENTENCODER_H
#include "classlist.h"
void baseBinaryIndexElementAssign(ElementEncoding *This);
void naiveEncodeFunctionPredicate(Encodings* encodings, FunctionEncoding *This);
void naiveEncodeCircuitFunction(Encodings* encodings,FunctionEncoding* This);
void naiveEncodeEnumeratedFunction(Encodings* encodings, FunctionEncoding* This);
void naiveEncodeEnumTableFunc(Encodings* encodings, ElementFunction* This);
void naiveEncodeEnumOperatingFunc(Encodings* encodings, ElementFunction* This);
#endif
