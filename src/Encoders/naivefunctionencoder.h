

#ifndef NAIVEFUNCTIONENCODER_H
#define NAIVEFUNCTIONENCODER_H
#include "encodings.h"
#include "functionencoding.h"

void naiveEncodeFunctionPredicate(Encodings* encodings, FunctionEncoding *This);
void naiveEncodeCircuitFunction(Encodings* encodings,FunctionEncoding* This);
void naiveEncodeEnumeratedFunction(Encodings* encodings, FunctionEncoding* This);
void naiveEncodeEnumTableFunc(Encodings* encodings, ElementFunction* This);
void naiveEncodeEnumOperatingFunc(Encodings* encodings, ElementFunction* This);
#endif /* NAIVEFUNCTIONENCODER_H */

