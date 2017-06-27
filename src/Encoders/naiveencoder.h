#ifndef NAIVEELEMENTENCODER_H
#define NAIVEELEMENTENCODER_H
#include "classlist.h"
#include "structs.h"

#define GETNAIVEENCODERVARS(ne) (&((NaiveEncoder*)ne)->vars)
#define GETNAIVEENCODERALLCONSTRAINTS(ne) (&((NaiveEncoder*)ne)->allConstraints)

struct NaiveEncoder{
	uint varindex;
	VectorConstraint vars;
	VectorConstraint allConstraints;
};

NaiveEncoder* allocNaiveEncoder();
Constraint* getNewVar(NaiveEncoder* encoder);
void getArrayNewVars(NaiveEncoder* encoder, uint num, Constraint **carray);
//For now, This function just simply goes through elements/functions and 
//assigns a predefined Encoding to each of them
void naiveEncodingDecision(CSolver* csolver, NaiveEncoder* encoder);
void encode(CSolver* csolver);
void baseBinaryIndexElementAssign(ElementEncoding *This);
void naiveEncodeFunctionPredicate(NaiveEncoder* encoder, FunctionEncoding *This);
void naiveEncodeCircuitFunction(NaiveEncoder* encoder, FunctionEncoding* This);
void naiveEncodeEnumeratedFunction(NaiveEncoder* encoder, FunctionEncoding* This);
void naiveEncodeEnumTableFunc(NaiveEncoder* encoder, ElementFunction* This);
void naiveEncodeEnumOperatingFunc(NaiveEncoder* encoder, ElementFunction* This);

void deleteNaiveEncoder(NaiveEncoder* encoder);
#endif
